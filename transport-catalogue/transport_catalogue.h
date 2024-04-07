#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "geo.h"

namespace transport {
	namespace transport_catalogue {
		struct Stop {
			std::string name;
			geo::Coordinates coordinates;
			std::deque<std::string_view> buses_for_stop;
		};

		struct Bus {
			std::string name;
			std::vector<Stop*> stops;
			size_t unique;
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
			void AddBus(std::string_view id, std::vector<std::string_view> stops);

			Stop* FindStop(std::string_view id) const;
			Bus* FindBus(std::string_view id) const;
			std::tuple<size_t, size_t, double> GetBusInfo(std::string_view id) const;

		private:
			std::deque<Stop> stops_;
			std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
			std::deque<Bus> buses_;
			std::unordered_map<std::string_view, Bus*> busname_to_bus_;
		};
	}
}