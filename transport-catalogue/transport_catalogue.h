#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "geo.h"
#include "domain.h"

namespace transport {
	namespace transport_catalogue {

		namespace detail {
			struct Hasher {
				size_t operator() (const std::pair<domain::Stop*, domain::Stop*>& d) const {
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

			domain::Stop* FindStop(std::string_view id) const;
			domain::Bus* FindBus(std::string_view id) const;
			domain::BusInfo GetBusInfo(std::string_view id) const;
			int GetDistance(domain::Stop* stop1, domain::Stop* stop2) const;

			std::deque<domain::Stop>::const_iterator StopsBeginIt() const {
				return stops_.begin();
			}

			std::deque<domain::Stop>::const_iterator StopsEndIt() const {
				return stops_.end();
			}

			std::deque<domain::Bus>::const_iterator BusesBeginIt() const {
				return buses_.begin();
			}

			std::deque<domain::Bus>::const_iterator BusesEndIt() const {
				return buses_.end();
			}

		private:
			std::deque<domain::Stop> stops_;
			std::unordered_map<std::string_view, domain::Stop*> stopname_to_stop_;
			std::deque<domain::Bus> buses_;
			std::unordered_map<std::string_view, domain::Bus*> busname_to_bus_;
			std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, detail::Hasher> distances_;
		};
	}
}