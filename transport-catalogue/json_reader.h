#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"

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

			void StopOutput(std::ostream& out, const json::Node& info, std::vector<json::Node>& nodes);
			void BusOutput(std::ostream& out, const json::Node& info, std::vector<json::Node>& nodes);
			void MapOutput(std::ostream& out, const json::Node& info, std::vector<json::Node>& nodes);

			void StopReader(transport_catalogue::TransportCatalogue& catalogue);
			void BusReader(transport_catalogue::TransportCatalogue& catalogue);

		private:
			transport_catalogue::TransportCatalogue& catalogue_;
			json::Document doc_;
		};
	}
}
