#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"
#include "json_builder.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace transport {
	namespace json_reader {

		struct Distance {
			std::string start_stop;
			std::string end_stop;
			int distance;
		};

		class Reader {
		public:

			explicit Reader(
				transport_catalogue::TransportCatalogue& catalogue,
				std::istream& in
			);

			void Output(std::ostream& out);
			domain::RenderSettings GetInfoXML();

			void StopReader(transport_catalogue::TransportCatalogue& catalogue);
			void BusReader(transport_catalogue::TransportCatalogue& catalogue);

			void StopOutput(const json::Node& info, std::vector<json::Node>& nodes);
			void BusOutput(const json::Node& info, std::vector<json::Node>& nodes);
			void MapOutput(const json::Node& info, std::vector<json::Node>& nodes);
			void RouteOutput(const json::Node& info, std::vector<json::Node>& nodes);

		private:
			transport_catalogue::TransportCatalogue& catalogue_;
			json::Document doc_;
		};
	}
}
