#pragma once

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

namespace transport {
	namespace transport_router {

		static const int SECONDS_FOR_MINUTE = 60;
		static const int METRS_FOR_KILOMETR = 1000;

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
				: catalogue_(catalogue) {
				BuildGraph();
				SetRouteValue();
			}

			RouteInfo GetRouteInfo(std::string_view stop1, std::string_view stop2) const;

		private:

			void BuildGraph();

			void SetRouteValue() {
				graph::Router<double> temp_router(weight_graph_);
				router_ = std::make_unique<graph::Router<double>>(temp_router);
			}

			graph::Router<double>* GetRouter() {
				return router_.get();
			}

			void AddRoute(size_t from, size_t to, double distance) {
				weight_graph_.AddEdge({ from, to, distance });
			}

			void AddEdgeInfo(domain::Bus* bus, int count) {
				edge_info_.push_back({ bus, count });
			}

		private:
			struct PassingStopsCount {
				domain::Bus* bus;
				int stops_count;
			};

			graph::DirectedWeightedGraph<double> weight_graph_;
			std::unique_ptr<graph::Router<double>> router_;
			std::deque<PassingStopsCount> edge_info_;
			const transport_catalogue::TransportCatalogue& catalogue_;
		};
	}
}