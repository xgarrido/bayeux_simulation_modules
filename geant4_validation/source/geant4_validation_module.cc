// geant4_validation_module.cc

#include <stdexcept>
#include <sstream>
#include <set>

#include <geant4_validation_module.h>

// Utilities
#include <datatools/clhep_units.h>

// SuperNEMO event model
#include <mctools/utils.h>

// Service manager
#include <datatools/service_manager.h>

// Services
#include <dpp/histogram_service.h>

// Histogram
#include <mygsl/histogram_pool.h>

namespace analysis {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(geant4_validation_module,
                                    "analysis::geant4_validation_module");

  // Set the histogram pool used by the module :
  void geant4_validation_module::set_histogram_pool (mygsl::histogram_pool & pool_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is already initialized !");
    _histogram_pool_ = &pool_;
    return;
  }

  // Grab the histogram pool used by the module :
  mygsl::histogram_pool & geant4_validation_module::grab_histogram_pool ()
  {
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is not initialized !");
    return *_histogram_pool_;
  }

  void geant4_validation_module::_set_defaults ()
  {
    _histogram_pool_ = 0;
    return;
  }

  // Initialization :
  void geant4_validation_module::initialize(const datatools::properties  & config_,
                                            datatools::service_manager   & service_manager_,
                                            dpp::module_handle_dict_type & module_dict_)
  {
    DT_THROW_IF (is_initialized (),
                 std::logic_error,
                 "Module '" << get_name () << "' is already initialized ! ");

    dpp::base_module::_common_initialize (config_);

    // Service label
    std::string histogram_label;
    if (config_.has_key ("Histo_label"))
      {
        histogram_label = config_.fetch_string ("Histo_label");
      }
    if (! _histogram_pool_)
      {
        DT_THROW_IF (histogram_label.empty (), std::logic_error,
                     "Module '" << get_name () << "' has no valid 'Histo_label' property !");

        DT_THROW_IF (! service_manager_.has (histogram_label) ||
                     ! service_manager_.is_a<dpp::histogram_service> (histogram_label),
                     std::logic_error,
                     "Module '" << get_name () << "' has no '" << histogram_label << "' service !");
        dpp::histogram_service & Histo
          = service_manager_.get<dpp::histogram_service> (histogram_label);
        set_histogram_pool (Histo.grab_pool ());
      }

    // Tag the module as initialized :
    _set_initialized (true);
    return;
  }

  // Reset :
  void geant4_validation_module::reset()
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Module '" << get_name () << "' is not initialized !");

    // Dump result
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG)
      {
        DT_LOG_NOTICE(get_logging_priority (), "geant4 validation dump: ");
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
    _set_defaults ();
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
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is not initialized !");

    // // Check if the 'particle track' record bank is available :
    // const std::string ptd_label = snemo::analysis::model::data_info::PARTICLE_TRACK_DATA_LABEL;
    // if (! data_record_.has (ptd_label))
    //   {
    //     DT_LOG_ERROR (get_logging_priority (), "Could not find any bank with label '"
    //                   << ptd_label << "' !");
    //     return dpp::base_module::PROCESS_STOP;
    //   }
    // const snemo::analysis::model::particle_track_data & ptd
    //   = data_record_.get<snemo::analysis::model::particle_track_data>(ptd_label);

    // if (get_logging_priority () >= datatools::logger::PRIO_DEBUG)
    //   {
    //     DT_LOG_DEBUG (get_logging_priority (), "Particle track data : ");
    //     ptd.tree_dump ();
    //   }

    // // Particle Counters
    // size_t nelectron  = 0;
    // size_t npositron  = 0;
    // size_t nundefined = 0;

    // // Calibrated energies
    // double total_energy = 0.0;

    // // Store geom_id to avoid double inclusion of energy deposited
    // std::set<geomtools::geom_id> gids;

    // // Loop over all saved particles
    // const snemo::analysis::model::particle_track_data::particle_collection_type &
    //   the_particles = ptd.get_particles ();

    // for (snemo::analysis::model::particle_track_data::particle_collection_type::const_iterator
    //        iparticle = the_particles.begin ();
    //      iparticle != the_particles.end ();
    //      ++iparticle)
    //   {
    //     const snemo::analysis::model::particle_track & a_particle = iparticle->get ();

    //     if (! a_particle.has_associated_calorimeters ())
    //       {
    //         DT_LOG_DEBUG(get_logging_priority(),
    //                      "Particle track is not associated to any calorimeter block !");
    //         continue;
    //       }

    //     const snemo::analysis::model::particle_track::calorimeter_collection_type &
    //       the_calorimeters = a_particle.get_associated_calorimeters ();

    //     if (the_calorimeters.size () > 2)
    //       {
    //         DT_LOG_DEBUG (get_logging_priority (),
    //                       "The particle is associated to more than 2 calorimeters !");
    //         continue;
    //       }

    //     for (size_t i = 0; i < the_calorimeters.size (); ++i)
    //       {
    //         const geomtools::geom_id & gid = the_calorimeters.at (i).get ().get_geom_id ();
    //         if (gids.find (gid) != gids.end ()) continue;
    //         gids.insert (gid);
    //         total_energy += the_calorimeters.at (i).get ().get_energy ();
    //       }

    //     if      (a_particle.get_charge () == snemo::analysis::model::particle_track::negative) nelectron++;
    //     else if (a_particle.get_charge () == snemo::analysis::model::particle_track::positive) npositron++;
    //     else nundefined++;
    //   }

    // // Build unique key for histogram map:
    // std::ostringstream key;

    // // Retrieving info from header bank:
    // const datatools::properties & eh_properties = eh.get_properties ();
    // for (std::vector<std::string>::const_iterator
    //        ifield = _key_fields_.begin ();
    //      ifield != _key_fields_.end (); ++ifield)
    //   {
    //     const std::string & a_field = *ifield;
    //     if (! eh_properties.has_key (a_field))
    //       {
    //         DT_LOG_WARNING (get_logging_priority (),
    //                         "No properties with key '" << a_field << "' "
    //                         << "has been found in event header !");
    //         continue;
    //       }

    //     if (eh_properties.is_vector (a_field))
    //       {
    //         DT_LOG_WARNING (get_logging_priority (),
    //                         "Stored properties '" << a_field << "' " << "must be scalar !");
    //         continue;
    //       }
    //     if (eh_properties.is_boolean (a_field))      key << eh_properties.fetch_boolean (a_field);
    //     else if (eh_properties.is_integer (a_field)) key << eh_properties.fetch_integer (a_field);
    //     else if (eh_properties.is_real (a_field))    key << eh_properties.fetch_real (a_field);
    //     else if (eh_properties.is_string (a_field))  key << eh_properties.fetch_string (a_field);

    //     // Add a underscore separator between fields
    //     key << KEY_FIELD_SEPARATOR;
    //   }
    // // Add charge multiplicity
    // key << nelectron << "e-" << npositron << "e+" << nundefined << "u";

    // DT_LOG_TRACE(get_logging_priority(), "Total energy = " << total_energy / CLHEP::keV << " keV");
    // DT_LOG_TRACE(get_logging_priority(), "Number of electrons = " << nelectron);
    // DT_LOG_TRACE(get_logging_priority(), "Number of positrons = " << npositron);
    // DT_LOG_TRACE(get_logging_priority(), "Number of undefined = " << nundefined);
    // DT_LOG_TRACE(get_logging_priority(), "Key = " << key.str());

    // // Arbitrary selection of "two-particles" channel
    // if (nelectron != 2)
    //   {
    //     DT_LOG_WARNING (get_logging_priority (), "Selecting only two-electrons events!");
    //     return dpp::base_module::PROCESS_CONTINUE;
    //   }

    // // Getting histogram pool
    // mygsl::histogram_pool & a_pool = grab_histogram_pool ();

    // if (! a_pool.has (key.str ()))
    //   {
    //     mygsl::histogram_1d & h = a_pool.add_1d (key.str (), "", "energy");
    //     datatools::properties hconfig;
    //     hconfig.store_string ("mode", "mimic");
    //     hconfig.store_string ("mimic.histogram_1d", "energy_template");
    //     mygsl::histogram_pool::init_histo_1d (h, hconfig, &a_pool);
    //   }

    // // Getting the current histogram
    // mygsl::histogram_1d & a_histo = a_pool.grab_1d (key.str ());
    // a_histo.fill (total_energy);

    // // Compute normalization factor given the total number of events generated
    // // and the weight of each event
    // double weight = 1.0;
    // if (eh_properties.has_key ("analysis.total_number_of_event"))
    //   {
    //     weight /= eh_properties.fetch_integer ("analysis.total_number_of_event");
    //   }
    // if (eh_properties.has_key (mctools::event_utils::EVENT_GENBB_WEIGHT))
    //   {
    //     weight *= eh_properties.fetch_real (mctools::event_utils::EVENT_GENBB_WEIGHT);
    //   }

    // // Store the weight into histogram properties
    // if (! a_histo.get_auxiliaries ().has_key ("weight"))
    //   {
    //     a_histo.grab_auxiliaries ().update ("weight", weight);
    //   }

    return dpp::base_module::PROCESS_SUCCESS;
  }

  void geant4_validation_module::dump_result (std::ostream      & out_,
                                              const std::string & title_,
                                              const std::string & indent_,
                                              bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    if ( !title_.empty () )
      {
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
