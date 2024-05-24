#pragma once

#include "geo.h"
#include "svg.h"
#include "transport_catalogue.h"
#include "domain.h"

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <optional>
#include <vector>

namespace transport {
	namespace map_renderer {
        inline const double EPSILON = 1e-6;

        bool IsZero(double value);

        class SphereProjector {
        public:

            template <typename PointInputIt>
            SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                double max_width, double max_height, double padding)
                : padding_(padding)
            {

                if (points_begin == points_end) {
                    return;
                }

                const auto [left_it, right_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
                min_lon_ = left_it->lng;
                const double max_lon = right_it->lng;

                const auto [bottom_it, top_it] = std::minmax_element(
                    points_begin, points_end,
                    [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
                const double min_lat = bottom_it->lat;
                max_lat_ = top_it->lat;

                std::optional<double> width_zoom;
                if (!IsZero(max_lon - min_lon_)) {
                    width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
                }

                std::optional<double> height_zoom;
                if (!IsZero(max_lat_ - min_lat)) {
                    height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
                }

                if (width_zoom && height_zoom) {
                    zoom_coeff_ = std::min(*width_zoom, *height_zoom);
                }
                else if (width_zoom) {
                    zoom_coeff_ = *width_zoom;
                }
                else if (height_zoom) {
                    zoom_coeff_ = *height_zoom;
                }
            }

            svg::Point operator()(geo::Coordinates coords) const {
                return {
                    (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_
                };
            }

        private:
            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };

        class MapRenderer {
        public:

            explicit MapRenderer(transport_catalogue::TransportCatalogue& catalogue, const domain::RenderSettings& xml);
            void Output(std::ostream& out);

            void BusRenderer(transport_catalogue::TransportCatalogue& catalogue, const domain::RenderSettings& xml, map_renderer::SphereProjector& sphere_projector);
            void StopRenderer(transport_catalogue::TransportCatalogue& catalogue, const domain::RenderSettings& xml, map_renderer::SphereProjector& sphere_projector);

        private:
            svg::Document doc_;
        };
	}
}