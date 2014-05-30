/* geant4_validation_module.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2014-05-30
 * Last modified : 2014-05-30
 *
 * Copyright (C) 2014 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *
 * A dedicated module to study and to validate Geant4 simulations
 *
 * History:
 *
 */

#ifndef ANALYSIS_GEANT4_VALIDATION_MODULE_H
#define ANALYSIS_GEANT4_VALIDATION_MODULE_H 1

// Data processing module abstract base class
#include <dpp/base_module.h>

#include <string>

namespace mygsl {
  class histogram_pool;
}

namespace analysis {

  class geant4_validation_module : public dpp::base_module
  {
  public:

    /// Setting histogram pool
    void set_histogram_pool(mygsl::histogram_pool & pool_);

    /// Grabbing histogram pool
    mygsl::histogram_pool & grab_histogram_pool();


    /// Constructor
    geant4_validation_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~geant4_validation_module();

    /// Initialization
    virtual void initialize(const datatools::properties  & setup_,
                            datatools::service_manager   & service_manager_,
                            dpp::module_handle_dict_type & module_dict_);

    /// Reset
    virtual void reset();

    /// Data record processing
    virtual process_status process(datatools::things & data_);

    void dump_result(std::ostream      & out_    = std::clog,
                     const std::string & title_  = "",
                     const std::string & indent_ = "",
                     bool inherit_               = false) const;

  protected:

    /// Give default values to specific class members.
    void _set_defaults();

  private:

    // The label/name of the 'Histo' service accessible from the service manager :
    std::string _Histo_service_label_;

    // The histogram pool :
    mygsl::histogram_pool * _histogram_pool_;

    // The label/name of the bank accessible from the event record :
    std::string _bank_label_;

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE (geant4_validation_module);

  };

} // namespace analysis

#endif // ANALYSIS_GEANT4_VALIDATION_MODULE_H

// end of geant4_validation_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
