#include "json_reader.h"

#include <algorithm>

namespace transport {
    namespace json_reader {

        void Reader::StopReader(transport_catalogue::TransportCatalogue& catalogue) {
            std::vector<Distance> distances;

            for (const auto& info : doc_.GetRoot().AsDict().at("base_requests").AsArray()) {
                if (info.AsDict().at("type").AsString() == "Stop") {
                    std::string TEST = info.AsDict().at("name").AsString();
                    catalogue.AddStop(
                        info.AsDict().at("name").AsString(),
                        {
                            info.AsDict().at("latitude").AsDouble(),
                            info.AsDict().at("longitude").AsDouble()
                        }
                    );

                    if (info.AsDict().count("road_distances") > 0) {
                        for (const auto& distance : info.AsDict().at("road_distances").AsDict()) {
                            distances.push_back({
                                info.AsDict().at("name").AsString(),
                                distance.first,
                                distance.second.AsInt()
                                });
                        }
                    }
                }
            }

            for (const auto& distance : distances) {
                catalogue.AddDistance(distance.start_stop, distance.end_stop, distance.distance);
            }
        }

        void Reader::BusReader(transport_catalogue::TransportCatalogue& catalogue) {
            for (const auto& info : doc_.GetRoot().AsDict().at("base_requests").AsArray()) {
                if (info.AsDict().at("type").AsString() == "Bus") {
                    std::vector<std::string_view> stops;

                    for (const auto& stop : info.AsDict().at("stops").AsArray()) {
                        stops.push_back(stop.AsString());
                    }

                    if (!info.AsDict().at("is_roundtrip").AsBool()) {
                        auto it = info.AsDict().at("stops").AsArray().rbegin() + 1;
                        while (it != info.AsDict().at("stops").AsArray().rend()) {
                            stops.push_back(it->AsString());
                            ++it;
                        }
                    }

                    catalogue.AddBus(
                        info.AsDict().at("name").AsString(),
                        stops, info.AsDict().at("is_roundtrip").AsBool()
                    );
                }
            }
        }

        Reader::Reader(
            transport_catalogue::TransportCatalogue& catalogue,
            std::istream& in
        )
            : catalogue_(catalogue), doc_(json::Load(in)) {

            using namespace json;

            StopReader(catalogue);
            BusReader(catalogue);
        }

        void Reader::StopOutput(const json::Node& info, std::vector<json::Node>& nodes) {
            using namespace json;
            using namespace std::literals;

            auto stop_info = catalogue_.FindStop(info.AsDict().at("name").AsString());
            if (!stop_info) {

                nodes.push_back(json::Builder{}
                    .StartDict()
                    .Key("error_message"s).Value(std::string("not found"))
                    .Key("request_id"s).Value(info.AsDict().at("id").GetValue())
                    .EndDict()
                    .Build()
                );

                /*Dict dict = {
                    {"error_message", std::string("not found")},
                    {"request_id", info.AsDict().at("id")}
                };
                nodes.push_back(std::move(dict));*/
            }
            else {
                auto buses_info = stop_info->buses_for_stop;
                std::sort(buses_info.begin(), buses_info.end());

                std::vector<Node> node_buses;
                for (const auto& bus : buses_info) {
                    node_buses.push_back(std::string(bus));
                }

                nodes.push_back(json::Builder{}
                    .StartDict()
                    .Key("buses"s).Value(node_buses)
                    .Key("request_id"s).Value(info.AsDict().at("id").GetValue())
                    .EndDict()
                    .Build()
                );

                /*Dict dict = {
                    {"buses", node_buses},
                    {"request_id", info.AsDict().at("id")}
                };*/

                //nodes.push_back(std::move(dict));
            }
        }

        void Reader::BusOutput(const json::Node& info, std::vector<json::Node>& nodes) {
            using namespace json;

            auto bus_info = catalogue_.GetBusInfo(info.AsDict().at("name").AsString());

            using namespace std::literals;

            if (bus_info.stops_on_route == 0) {

                nodes.push_back(json::Builder{}
                    .StartDict()
                    .Key("error_message"s).Value(std::string("not found"))
                    .Key("request_id"s).Value(info.AsDict().at("id").GetValue())
                    .EndDict()
                    .Build()
                );

                /*Dict dict = {
                    {"error_message", std::string("not found")},
                    {"request_id", info.AsDict().at("id")}
                };*/

                //nodes.push_back(std::move(dict));
            }
            else {

                nodes.push_back(json::Builder{}
                    .StartDict()
                    .Key("curvature"s).Value(bus_info.curvature)
                    .Key("request_id"s).Value(info.AsDict().at("id").GetValue())
                    .Key("route_length"s).Value(bus_info.route_length)
                    .Key("stop_count"s).Value(static_cast<int>(bus_info.stops_on_route))
                    .Key("unique_stop_count"s).Value(static_cast<int>(bus_info.unique_stops))
                    .EndDict()
                    .Build()
                );

                /*Dict dict = {
                    {"curvature", bus_info.curvature},
                    {"request_id", info.AsDict().at("id")},
                    {"route_length", bus_info.route_length},
                    {"stop_count", static_cast<int>(bus_info.stops_on_route)},
                    {"unique_stop_count", static_cast<int>(bus_info.unique_stops)}
                };

                nodes.push_back(std::move(dict));*/
            }
        }

        void Reader::MapOutput(const json::Node& info, std::vector<json::Node>& nodes) {
            using namespace json;
            using namespace std::literals;

            std::ostringstream os;
            transport::map_renderer::MapRenderer reader_xml(catalogue_, GetInfoXML());
            reader_xml.Output(os);

            /*Dict dict = {
                {"map", os.str()},
                {"request_id", info.AsDict().at("id")}
            };
            nodes.push_back(std::move(dict));*/

            nodes.push_back(json::Builder{}
                .StartDict()
                .Key("map"s).Value(os.str())
                .Key("request_id"s).Value(info.AsDict().at("id").GetValue())
                .EndDict()
                .Build()
            );
        }

        void Reader::Output(std::ostream& out) {
            using namespace json;

            std::vector<Node> nodes;

            for (const auto& info : doc_.GetRoot().AsDict().at("stat_requests").AsArray()) {
                if (info.AsDict().at("type").AsString() == "Stop") {
                    StopOutput(info, nodes);
                }

                if (info.AsDict().at("type").AsString() == "Bus") {
                    BusOutput(info, nodes);
                }

                if (info.AsDict().at("type").AsString() == "Map") {
                    MapOutput(info, nodes);
                }
            }

            Document doc(nodes);
            Print(doc, out);
        }

        svg::Color GetColor(json::Node& node) {
            svg::Color color;

            if (node.IsString()) {
                color = svg::Color(node.AsString());
            }
            else if (node.IsArray() && node.AsArray().size() == 3) {
                color = svg::Color(svg::Rgb(
                    static_cast<uint8_t>(node.AsArray().at(0).AsDouble()),
                    static_cast<uint8_t>(node.AsArray().at(1).AsDouble()),
                    static_cast<uint8_t>(node.AsArray().at(2).AsDouble())
                ));
            }
            else {
                color = svg::Color(svg::Rgba(
                    static_cast<uint8_t>(node.AsArray().at(0).AsDouble()),
                    static_cast<uint8_t>(node.AsArray().at(1).AsDouble()),
                    static_cast<uint8_t>(node.AsArray().at(2).AsDouble()),
                    node.AsArray().at(3).AsDouble()
                ));
            }

            return color;
        }

        std::vector<svg::Color> GetVectorColor(std::vector<json::Node>& nodes) {
            std::vector<svg::Color> colors;

            for (auto& node : nodes) {
                colors.push_back(GetColor(node));
            }

            return colors;
        }

        domain::RenderSettings Reader::GetInfoXML() {
            auto& info = doc_.GetRoot().AsDict().at("render_settings").AsDict();

            std::vector<json::Node> bus_label_offset = info.at("bus_label_offset").AsArray();
            std::vector<json::Node> stop_label_offset = info.at("stop_label_offset").AsArray();

            json::Node underlayer_color(info.at("underlayer_color"));

            std::vector<json::Node> color_palette(info.at("color_palette").AsArray());

            return {
                info.at("width").AsDouble(),
                info.at("height").AsDouble(),
                info.at("padding").AsDouble(),
                info.at("line_width").AsDouble(),
                info.at("stop_radius").AsDouble(),
                info.at("bus_label_font_size").AsInt(),
                {
                    bus_label_offset.at(0).AsDouble(),
                    bus_label_offset.at(1).AsDouble()
                },
                info.at("stop_label_font_size").AsInt(),
                {
                    stop_label_offset.at(0).AsDouble(),
                    stop_label_offset.at(1).AsDouble()
                },
                GetColor(underlayer_color),
                info.at("underlayer_width").AsDouble(),
                GetVectorColor(color_palette)
            };
        }
    }
}