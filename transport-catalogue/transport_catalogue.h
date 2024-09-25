#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "geo.h"
#include "domain.h"
#include "graph.h"
#include "router.h"

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

			struct EdgeInfo {
				double weight;
				int span_count;
				std::string type;
				std::string_view name;
			};

			struct RouteInfo {
				std::vector<EdgeInfo> edges;
				double total_time;
			};

			void AddStop(std::string_view id, geo::Coordinates coordinates);
			void AddDistance(std::string_view id, std::string_view stop, int distance);
			void AddBus(std::string_view id, const std::vector<std::string_view>& stops, bool is_round);

			domain::Stop* FindStop(std::string_view id) const;
			domain::Bus* FindBus(std::string_view id) const;
			domain::BusInfo GetBusInfo(std::string_view id) const;
			RouteInfo GetRouteInfo(std::string_view stop1, std::string_view stop2) const;
			int GetDistance(domain::Stop* stop1, domain::Stop* stop2) const;
			size_t GetStopsCount() const {
				return stops_.size();
			}

			size_t GetStopId(std::string_view id) const;

			void SetEdgeValue() {
				weightGraph = graph::DirectedWeightedGraph<double>(stops_.size() * 2);
			}

			void SetRouteValue() {
				graph::Router<double> temp_router(weightGraph);
				router_ = std::make_unique<graph::Router<double>>(temp_router);
			}

			graph::Router<double>* GetRouter() {
				return router_.get();
			}

			void AddRoute(size_t from, size_t to, double distance) {
				weightGraph.AddEdge({ from, to, distance });
			}

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

			void AddEdgeInfo(domain::Bus* bus, int count) {
				edgeInfo_.push_back({ bus, count });
			}

		private:
			std::deque<domain::Stop> stops_;
			std::unordered_map<std::string_view, domain::Stop*> stopname_to_stop_;
			std::unordered_map<std::string_view, size_t> stopindex_to_stop_;
			std::deque<domain::Bus> buses_;
			std::unordered_map<std::string_view, domain::Bus*> busname_to_bus_;
			std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, detail::Hasher> distances_;
			graph::DirectedWeightedGraph<double> weightGraph;
			std::unique_ptr<graph::Router<double>> router_;
			std::deque<std::pair<domain::Bus*, int>> edgeInfo_;
		};
	}
}