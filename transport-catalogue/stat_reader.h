#pragma once

#include <deque>
#include <iosfwd> // TODO
#include <optional>
#include <string_view>
#include <tuple>

#include "transport_catalogue.h"

namespace transport {
	namespace stat_reader {
		namespace detail {
			void BusPrintStat(std::string_view request,
				std::ostream& output,
				std::tuple<size_t, size_t, double> info);

			void StopPrintStat(std::string_view request,
				std::ostream& output,
				std::optional<std::deque<std::string_view>> info);
		}

		void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& transport_catalogue,
			std::string_view request,
			std::ostream& output);
	}
}