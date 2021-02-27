/* nvcvcam.hpp -- NvCvCam
 *
 * Copyright (C) 2020 Michael de Gans
 *
 * This is a usage example or test and hereby public domain.
 */

#include "nvcvcam_error.hpp"

#include "nvcvcam.hpp"

#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/utility/setup/file.hpp>

#include <assert.h>

void setup_logging() {
  boost::log::register_simple_formatter_factory<
      boost::log::trivial::severity_level, char>("Severity");
  boost::log::add_file_log(
      LOGFILE, boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
  boost::log::add_console_log(
      std::cout,
      boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
  boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                      boost::log::trivial::debug);
  boost::log::add_common_attributes();
}

int main() {
  setup_logging();

  BOOST_LOG_TRIVIAL(info) << TESTNAME << ":start of Argus stress test";

  nvcvcam::NvCvCam camera;

  // It's not enough to test that the camera opens and closes since it's
  // possible for the argus daemon to get stuck. We need to ensure that this
  // cannot happen, since the daemon itself apparently cannot.
  for (size_t i = 0; i < 5; i++) {
    assert(camera.open());
    assert(camera.close());
  }

  BOOST_LOG_TRIVIAL(info) << TESTNAME << ":end of Argus stress test";
  return 0;
}