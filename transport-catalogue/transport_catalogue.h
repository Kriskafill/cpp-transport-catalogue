#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "geo.h"

namespace transport {
	namespace transport_catalogue {
		struct Stop {
			std::string name;
			geo::Coordinates coordinates;
			std::vector<std::string_view> buses_for_stop;
		};

		struct Bus {
			std::string name;
			std::vector<Stop*> stops;
			size_t unique;
			bool is_round;
		};

		struct BusInfo {
			size_t stops_on_route;
			size_t unique_stops;
			double route_length;
			double curvature;
		};

		namespace detail {
			struct Hasher {
				size_t operator() (const std::pair<Stop*, Stop*>& d) const {
					return static_cast<size_t>(
						d.first->coordinates.lat * 37 * 37 * 37 +
						d.first->coordinates.lng * 37 * 37 +
						d.second->coordinates.lat * 37 +
						d.second->coordinates.lng
						);
				}
			};
		}

		class TransportCatalogue {
		public:

			void AddStop(std::string_view id, geo::Coordinates coordinates);
			void AddDistance(std::string_view id, std::string_view stop, int distance);
			void AddBus(std::string_view id, const std::vector<std::string_view>& stops, bool is_round);

			Stop* FindStop(std::string_view id) const;
			Bus* FindBus(std::string_view id) const;
			BusInfo GetBusInfo(std::string_view id) const;
			int GetDistance(Stop* stop1, Stop* stop2) const;

			std::deque<Stop>::const_iterator StopsBeginIt() const {
				return stops_.begin();
			}

			std::deque<Stop>::const_iterator StopsEndIt() const {
				return stops_.end();
			}

			std::deque<Bus>::const_iterator BusesBeginIt() const {
				return buses_.begin();
			}

			std::deque<Bus>::const_iterator BusesEndIt() const {
				return buses_.end();
			}

		private:
			std::deque<Stop> stops_;
			std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
			std::deque<Bus> buses_;
			std::unordered_map<std::string_view, Bus*> busname_to_bus_;
			std::unordered_map<std::pair<Stop*, Stop*>, int, detail::Hasher> distances_;
		};
	}
}