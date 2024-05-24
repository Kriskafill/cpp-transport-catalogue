#include "map_renderer.h"

namespace transport {
    namespace map_renderer {

        bool IsZero(double value) {
            return std::abs(value) < EPSILON;
        }

        void MapRenderer::BusRenderer(transport_catalogue::TransportCatalogue& catalogue, const domain::RenderSettings& xml, map_renderer::SphereProjector& sphere_projector) {

            int index_color = 0;
            int color_count = xml.color_palette.size();

            std::deque<std::string_view> buses;
            for (auto it = catalogue.BusesBeginIt(); it != catalogue.BusesEndIt(); ++it) {
                buses.push_back(it->name);
            }

            std::sort(buses.begin(), buses.end());

            for (auto it = buses.begin(); it != buses.end(); ++it) {

                svg::Polyline polyline;
                polyline.SetStrokeColor(xml.color_palette.at(index_color));
                polyline.SetFillColor("none");
                polyline.SetStrokeWidth(xml.line_width);
                polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                polyline.SetStrokeColor(xml.color_palette.at(index_color));

                for (auto& stop : catalogue.FindBus(*it)->stops) {
                    polyline.AddPoint(sphere_projector(stop->coordinates));
                }

                if (catalogue.FindBus(*it)->stops.size() > 0) {
                    if (index_color + 1 < color_count) ++index_color;
                    else index_color = 0;

                    doc_.Add(polyline);
                }
            }

            index_color = 0;

            for (auto it = buses.begin(); it != buses.end(); ++it) {

                svg::Text text_underlayer;
                text_underlayer.SetPosition(sphere_projector(catalogue.FindBus(*it)->stops.at(0)->coordinates));
                text_underlayer.SetOffset(xml.bus_label_offset);
                text_underlayer.SetFontSize(xml.bus_label_font_size);
                text_underlayer.SetFontFamily("Verdana");
                text_underlayer.SetFontWeight("bold");
                text_underlayer.SetData(std::string(*it));
                text_underlayer.SetFillColor(xml.underlayer_color);
                text_underlayer.SetStrokeColor(xml.underlayer_color);
                text_underlayer.SetStrokeWidth(xml.underlayer_width);
                text_underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                text_underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                svg::Text text;
                text.SetPosition(sphere_projector(catalogue.FindBus(*it)->stops.at(0)->coordinates));
                text.SetOffset(xml.bus_label_offset);
                text.SetFontSize(xml.bus_label_font_size);
                text.SetFontFamily("Verdana");
                text.SetFontWeight("bold");
                text.SetData(std::string(*it));
                text.SetFillColor(xml.color_palette.at(index_color));

                svg::Text text_underlayer_line;
                svg::Text text_line;

                int size = catalogue.FindBus(*it)->stops.size();
                if (!catalogue.FindBus(*it)->is_round &&
                    size > 1 &&
                    catalogue.FindBus(*it)->stops.at(0)->name != catalogue.FindBus(*it)->stops.at(size / 2)->name) {

                    text_underlayer_line.SetPosition(sphere_projector(catalogue.FindBus(*it)->stops.at(size / 2)->coordinates));
                    text_underlayer_line.SetOffset(xml.bus_label_offset);
                    text_underlayer_line.SetFontSize(xml.bus_label_font_size);
                    text_underlayer_line.SetFontFamily("Verdana");
                    text_underlayer_line.SetFontWeight("bold");
                    text_underlayer_line.SetData(std::string(*it));
                    text_underlayer_line.SetFillColor(xml.underlayer_color);
                    text_underlayer_line.SetStrokeColor(xml.underlayer_color);
                    text_underlayer_line.SetStrokeWidth(xml.underlayer_width);
                    text_underlayer_line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                    text_underlayer_line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                    text_line.SetPosition(sphere_projector(catalogue.FindBus(*it)->stops.at(size / 2)->coordinates));
                    text_line.SetOffset(xml.bus_label_offset);
                    text_line.SetFontSize(xml.bus_label_font_size);
                    text_line.SetFontFamily("Verdana");
                    text_line.SetFontWeight("bold");
                    text_line.SetData(std::string(*it));
                    text_line.SetFillColor(xml.color_palette.at(index_color));
                }

                if (catalogue.FindBus(*it)->stops.size() > 0) {
                    if (index_color + 1 < color_count) ++index_color;
                    else index_color = 0;

                    doc_.Add(text_underlayer);
                    doc_.Add(text);

                    if (!catalogue.FindBus(*it)->is_round &&
                        size > 1 &&
                        catalogue.FindBus(*it)->stops.at(0)->name != catalogue.FindBus(*it)->stops.at(size / 2)->name) {

                        doc_.Add(text_underlayer_line);
                        doc_.Add(text_line);
                    }
                }
            }
        }

        void MapRenderer::StopRenderer(transport_catalogue::TransportCatalogue& catalogue, const domain::RenderSettings& xml, map_renderer::SphereProjector& sphere_projector) {

            std::deque<std::string_view> stops;
            for (auto it = catalogue.StopsBeginIt(); it != catalogue.StopsEndIt(); ++it) {
                stops.push_back(it->name);
            }

            std::sort(stops.begin(), stops.end());

            for (auto it = stops.begin(); it != stops.end(); ++it) {

                svg::Circle circle;
                circle.SetRadius(xml.stop_radius);
                circle.SetFillColor("white");

                if (catalogue.FindStop(*it)->buses_for_stop.size() > 0) {
                    circle.SetCenter(sphere_projector(catalogue.FindStop(*it)->coordinates));
                    doc_.Add(circle);
                }
            }

            for (auto it = stops.begin(); it != stops.end(); ++it) {

                svg::Text text;
                text.SetOffset(xml.stop_label_offset);
                text.SetFontSize(xml.stop_label_font_size);
                text.SetFontFamily("Verdana");
                text.SetFillColor(xml.underlayer_color);
                text.SetStrokeColor(xml.underlayer_color);
                text.SetStrokeWidth(xml.underlayer_width);
                text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                svg::Text text_2;
                text_2.SetOffset(xml.stop_label_offset);
                text_2.SetFontSize(xml.stop_label_font_size);
                text_2.SetFontFamily("Verdana");
                text_2.SetFillColor("black");

                if (catalogue.FindStop(*it)->buses_for_stop.size() > 0) {
                    text.SetPosition(sphere_projector(catalogue.FindStop(*it)->coordinates));
                    text.SetData(std::string(*it));
                    doc_.Add(text);

                    text_2.SetPosition(sphere_projector(catalogue.FindStop(*it)->coordinates));
                    text_2.SetData(std::string(*it));
                    doc_.Add(text_2);
                }
            }
        }

        MapRenderer::MapRenderer(transport_catalogue::TransportCatalogue& catalogue, const domain::RenderSettings& xml) {

            std::vector<geo::Coordinates> vect;

            for (auto it = catalogue.StopsBeginIt(); it != catalogue.StopsEndIt(); ++it) {
                if (it->buses_for_stop.size() > 0) {
                    vect.push_back(it->coordinates);
                }
            }

            map_renderer::SphereProjector sphere_projector(
                vect.begin(), vect.end(),
                xml.width, xml.height, xml.padding
            );
            
            BusRenderer(catalogue, xml, sphere_projector);
            StopRenderer(catalogue, xml, sphere_projector);
        }

        void MapRenderer::Output(std::ostream& out) {
            doc_.Render(out);
        }

    }
}