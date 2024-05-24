#include "transport_catalogue.h"

#include <algorithm>

namespace transport {
	namespace transport_catalogue {
		void TransportCatalogue::AddStop(std::string_view id, geo::Coordinates coordinates) {
			stops_.push_back({ std::move(std::string(id)), coordinates, {} });
			stopname_to_stop_.insert({ stops_.back().name, &stops_.back() });
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

		int TransportCatalogue::GetDistance(domain::Stop* stop1, domain::Stop* stop2) const {
			return distances_.at(std::pair<domain::Stop*, domain::Stop*>(stop1, stop2));
		}
	}
}