#pragma once

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

namespace transport {
	namespace transport_router {

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

		class TransportRouter {
		public:

			explicit TransportRouter(const transport_catalogue::TransportCatalogue& catalogue)
				: catalogue_(catalogue) {}

			void SetEdgeValue();

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

			void AddEdgeInfo(domain::Bus* bus, int count) {
				edgeInfo_.push_back({ bus, count });
			}

			RouteInfo GetRouteInfo(std::string_view stop1, std::string_view stop2) const;

		private:
			graph::DirectedWeightedGraph<double> weightGraph;
			std::unique_ptr<graph::Router<double>> router_;
			std::deque<std::pair<domain::Bus*, int>> edgeInfo_;
			const transport_catalogue::TransportCatalogue& catalogue_;
		};
	}
}