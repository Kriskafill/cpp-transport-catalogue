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

		void TransportCatalogue::AddBus(std::string_view id, const std::vector<std::string_view>& stops) {
			std::vector<Stop*> stops_vect;
			std::vector<Stop*> stops_vect_sort;
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

			buses_.push_back({ std::move(std::string(id)), std::move(stops_vect), unique });

			(*stops_vect_sort.begin())->buses_for_stop.push_back(buses_.back().name);
			for (auto it = stops_vect_sort.begin() + 1; it != stops_vect_sort.end(); ++it) {
				if ((*it)->name != (*(it - 1))->name) {
					(*it)->buses_for_stop.push_back(buses_.back().name);
				}
			}

			busname_to_bus_.insert({ buses_.back().name, &buses_.back() });
		}

		Stop* TransportCatalogue::FindStop(std::string_view id) const {
			if (stopname_to_stop_.count(id) == 0) {
				return nullptr;
			}
			else {
				return stopname_to_stop_.at(id);
			}
		}

		Bus* TransportCatalogue::FindBus(std::string_view id) const {
			if (busname_to_bus_.count(id) == 0) {
				return nullptr;
			}
			else {
				return busname_to_bus_.at(id);
			}
		}

		BusInfo TransportCatalogue::GetBusInfo(std::string_view id) const {
			Bus* bus = FindBus(id);
			if (bus == nullptr) {
				return { 0, 0, 0.0, 0.0 };
			}

			size_t R = bus->stops.size();
			size_t U = bus->unique;

			double L = 0;
			double L_in_line = 0;
			for (auto it = bus->stops.begin() + 1; it != bus->stops.end(); ++it) {

				if (distances_.count({ *(it - 1), *(it) }) > 0) {
					L += distances_.at({ *(it - 1), *(it) });
				}
				else {
					L += distances_.at({ *(it), *(it - 1) });
				}

				L_in_line += geo::ComputeDistance((*it)->coordinates, (*(it - 1))->coordinates);
			}

			double C = L / L_in_line;

			return { R, U, L, C };
		}
	}
}