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
			map_renderer::InfoXML GetInfoXML();

		private:
			transport_catalogue::TransportCatalogue& catalogue_;
			json::Document doc_;
		};
	}
}
