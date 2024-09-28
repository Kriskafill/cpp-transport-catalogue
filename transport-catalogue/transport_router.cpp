#include "transport_router.h"

namespace transport {
	namespace transport_router {

        void TransportRouter::SetEdgeValue() {
            weightGraph = graph::DirectedWeightedGraph<double>(catalogue_.GetStops().size() * 2);

            auto& r = catalogue_.GetBuses();

            for (int index = 0; index < r.size(); ++index) {
                for (auto i = r[index].stops.begin() + 1; i != r[index].stops.end(); ++i) {

                    double distance = 0.0;

                    int diff = 1;
                    for (auto it = i; it != r[index].stops.begin(); --it) {

                        distance += catalogue_.GetDistance(
                            catalogue_.FindStop((*(prev(it)))->name),
                            catalogue_.FindStop((*it)->name)
                        );

                        size_t prev_stop = catalogue_.GetStopId((*(prev(it)))->name);
                        size_t this_stop = catalogue_.GetStopId((*i)->name);

                        AddRoute(
                            prev_stop * 2 + 1,
                            this_stop * 2,
                            distance / 1000 / catalogue_.bus_velocity * 60
                        );

                        AddEdgeInfo(const_cast<domain::Bus*>(&(r[index])), diff);
                        ++diff;
                    }
                }
            }

            for (size_t i = 0; i < catalogue_.GetStopsCount(); ++i) {
                AddRoute(i * 2, i * 2 + 1, catalogue_.bus_wait_time);
                AddEdgeInfo(nullptr, 0);
            }
        }

		RouteInfo TransportRouter::GetRouteInfo(std::string_view stop1, std::string_view stop2) const {

			RouteInfo result;
			auto info_router = router_->BuildRoute(catalogue_.GetStopId(stop1) * 2, catalogue_.GetStopId(stop2) * 2);

			if (info_router) {

				for (int i = 0; i < info_router->edges.size(); ++i) {
					auto edge = edgeInfo_.at(info_router->edges[i]);
					if (!edge.first) {
						result.edges.push_back({
							weightGraph.GetEdge(info_router->edges[i]).weight,
							0,
							"Wait",
							std::string_view(catalogue_.GetStops().at(weightGraph.GetEdge(info_router->edges[i]).from / 2).name)
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

	}
}