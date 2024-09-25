#include "transport_catalogue.h"
//#include "log_duration.h"

#include <algorithm>

namespace transport {
	namespace transport_catalogue {
		void TransportCatalogue::AddStop(std::string_view id, geo::Coordinates coordinates) {
			stops_.push_back({ std::move(std::string(id)), coordinates, {} });
			stopname_to_stop_.insert({ stops_.back().name, &stops_.back() });
			stopindex_to_stop_.insert({ stops_.back().name, stops_.size() - 1});
		}

		void TransportCatalogue::AddDistance(std::string_view id, std::string_view stop, int distance) {
			distances_.insert({ { FindStop(id), FindStop(stop)}, distance });
		}

		void TransportCatalogue::AddBus(std::string_view id, const std::vector<std::string_view>& stops, bool is_round) {
			std::vector<domain::Stop*> stops_vect;
			std::vector<domain::Stop*> stops_vect_sort;
			for (const auto& stop : stops) {
				stops_vect.push_back(FindStop(stop));
				stops_vect_sort.push_back(FindStop(stop));
			}
			std::sort(stops_vect_sort.begin(), stops_vect_sort.end());

			size_t unique = 1;
			for (auto it = stops_vect_sort.begin() + 1; it != stops_vect_sort.end(); ++it) {
				if ((*it)->name != (*(it - 1))->name) {
					++unique;
				}
			}

			buses_.push_back({ std::move(std::string(id)), std::move(stops_vect), unique, is_round });

			(*stops_vect_sort.begin())->buses_for_stop.push_back(buses_.back().name);
			for (auto it = stops_vect_sort.begin() + 1; it != stops_vect_sort.end(); ++it) {
				if ((*it)->name != (*(it - 1))->name) {
					(*it)->buses_for_stop.push_back(buses_.back().name);
				}
			}

			busname_to_bus_.insert({ buses_.back().name, &buses_.back() });
		}

		domain::Stop* TransportCatalogue::FindStop(std::string_view id) const {
			if (stopname_to_stop_.count(id) == 0) {
				return nullptr;
			}
			else {
				return stopname_to_stop_.at(id);
			}
		}

		size_t TransportCatalogue::GetStopId(std::string_view id) const {
			return stopindex_to_stop_.at(id);
		}

		domain::Bus* TransportCatalogue::FindBus(std::string_view id) const {
			if (busname_to_bus_.count(id) == 0) {
				return nullptr;
			}
			else {
				return busname_to_bus_.at(id);
			}
		}

		domain::BusInfo TransportCatalogue::GetBusInfo(std::string_view id) const {
			domain::Bus* bus = FindBus(id);
			if (bus == nullptr) {
				return { 0, 0, 0.0, 0.0 };
			}

			size_t stops_on_route = bus->stops.size();
			size_t unique_stops = bus->unique;

			double route_length = 0;
			double route_length_in_line = 0;
			for (auto it = bus->stops.begin() + 1; it != bus->stops.end(); ++it) {

				if (distances_.count({ *(it - 1), *(it) }) > 0) {
					route_length += distances_.at({ *(it - 1), *(it) });
				}
				else {
					route_length += distances_.at({ *(it), *(it - 1) });
				}

				route_length_in_line += geo::ComputeDistance((*it)->coordinates, (*(it - 1))->coordinates);
			}

			double curvature = route_length / route_length_in_line;

			return { stops_on_route, unique_stops, route_length, curvature };
		}

		TransportCatalogue::RouteInfo TransportCatalogue::GetRouteInfo(std::string_view stop1, std::string_view stop2) const {
			
			//LOG_DURATION("GET_ROUTE_INFO");
			//graph::Router<double> router_(weightGraph);
			//LOG_DURATION("GET_ROUTE_INFO-WITHOUT-INITIALIZE");
			TransportCatalogue::RouteInfo result;
			auto info_router = router_->BuildRoute(GetStopId(stop1) * 2, GetStopId(stop2) * 2);

			if (info_router) {

				for (int i = 0; i < info_router->edges.size(); ++i) {
					auto edge = edgeInfo_.at(info_router->edges[i]);
					if (!edge.first) {
						result.edges.push_back({
							weightGraph.GetEdge(info_router->edges[i]).weight,
							0,
							"Wait",
							std::string_view(stops_.at(weightGraph.GetEdge(info_router->edges[i]).from / 2).name)
							});
					}
					else {
						result.edges.push_back({
							weightGraph.GetEdge(info_router->edges[i]).weight,
							edge.second,
							"Bus",
							std::string_view(edge.first->name)
							});
					}
				}

				result.total_time = info_router->weight;
			}
			else {
				result.total_time = -1;
			}

			return result;
		}

		int TransportCatalogue::GetDistance(domain::Stop* stop1, domain::Stop* stop2) const {
			if (distances_.count(std::pair<domain::Stop*, domain::Stop*>(stop1, stop2)) > 0) {
				return distances_.at(std::pair<domain::Stop*, domain::Stop*>(stop1, stop2));
			}
			else {
				return distances_.at(std::pair<domain::Stop*, domain::Stop*>(stop2, stop1));
			}
		}
	}
}