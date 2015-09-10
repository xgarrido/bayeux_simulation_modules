// geant4_validation_module.cc

// Standard libraries:
#include <stdexcept>
#include <sstream>

// Ourselves
#include <geant4_validation_module.h>

// Third party:
// - Bayeux/datatools
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/service_manager.h>
// - Bayeux/mctools
#include <bayeux/mctools/utils.h>
// - Bayeux/dpp
#include <bayeux/dpp/histogram_service.h>
// - Bayeux/mygsl
#include <bayeux/mygsl/histogram_pool.h>

namespace analysis {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(geant4_validation_module,
                                    "analysis::geant4_validation_module");

  // Set the histogram pool used by the module :
  void geant4_validation_module::set_histogram_pool(mygsl::histogram_pool & pool_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Module '" << get_name() << "' is already initialized !");
    _histogram_pool_ = &pool_;
    return;
  }

  // Grab the histogram pool used by the module :
  mygsl::histogram_pool & geant4_validation_module::grab_histogram_pool()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Module '" << get_name() << "' is not initialized !");
    return *_histogram_pool_;
  }

  void geant4_validation_module::_set_defaults()
  {
    _histogram_pool_ = 0;
    return;
  }

  // Initialization :
  void geant4_validation_module::initialize(const datatools::properties  & config_,
                                            datatools::service_manager   & service_manager_,
                                            dpp::module_handle_dict_type & module_dict_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module '" << get_name() << "' is already initialized ! ");

    dpp::base_module::_common_initialize(config_);

    // Service label
    std::string histogram_label;
    if (config_.has_key("Histo_label"))
      {
        histogram_label = config_.fetch_string("Histo_label");
      }
    if (! _histogram_pool_)
      {
        DT_THROW_IF(histogram_label.empty(), std::logic_error,
                    "Module '" << get_name() << "' has no valid 'Histo_label' property !");

        DT_THROW_IF(! service_manager_.has(histogram_label) ||
                    ! service_manager_.is_a<dpp::histogram_service>(histogram_label),
                    std::logic_error,
                    "Module '" << get_name() << "' has no '" << histogram_label << "' service !");
        dpp::histogram_service & Histo
          = service_manager_.get<dpp::histogram_service>(histogram_label);
        set_histogram_pool(Histo.grab_pool());
      }

    // Tag the module as initialized :
    _set_initialized(true);
    return;
  }

  // Reset :
  void geant4_validation_module::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Module '" << get_name() << "' is not initialized !");

    // Dump result
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG)
      {
        DT_LOG_NOTICE(get_logging_priority(), "geant4 validation dump: ");
        dump_result();
      }

    // Tag the module as un-initialized :
    _set_initialized(false);
    _set_defaults();
    return;
  }

  // Constructor :
  geant4_validation_module::geant4_validation_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_)
  {
    _set_defaults();
    return;
  }

  // Destructor :
  geant4_validation_module::~geant4_validation_module()
  {
    if (is_initialized()) geant4_validation_module::reset();
    return;
  }

  // Processing :
  dpp::base_module::process_status geant4_validation_module::process(datatools::things & data_record_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Module '" << get_name() << "' is not initialized !");

    return dpp::base_module::PROCESS_SUCCESS;
  }

  void geant4_validation_module::dump_result(std::ostream      & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) {
      indent = indent_;
    }
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    return;
  }

} // namespace analysis

// end of geant4_validation_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
