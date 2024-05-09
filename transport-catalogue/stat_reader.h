#pragma once

#include <deque>
#include <iostream>
#include <optional>
#include <string_view>

#include "transport_catalogue.h"

namespace transport {
	namespace stat_reader {
		namespace detail {
			void BusPrintStat(std::string_view request,
				std::ostream& output,
				transport_catalogue::BusInfo info);

			void StopPrintStat(std::string_view request,
				std::ostream& output,
				std::optional<std::deque<std::string_view>> info);
		}

		void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& transport_catalogue,
			std::string_view request,
			std::ostream& output);

		void DisplayTransportCatalogue(transport_catalogue::TransportCatalogue catalogue,
			std::istream& in,
			std::ostream& on);
	}
}