#pragma once

#include "geo.h"
#include "svg.h"

#include <string>
#include <vector>

namespace transport {
	namespace domain {
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

		struct RenderSettings {

			double width;
			double height;

			double padding;
			double line_width;
			double stop_radius;

			int bus_label_font_size;
			svg::Point bus_label_offset;

			int stop_label_font_size;
			svg::Point stop_label_offset;

			svg::Color underlayer_color;
			double underlayer_width;

			std::vector<svg::Color> color_palette;
		};
	}
}