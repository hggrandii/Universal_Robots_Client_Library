// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// Copyright 2019 FZI Forschungszentrum Informatik
// Created on behalf of Universal Robots A/S
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Felix Exner exner@fzi.de
 * \date    2019-04-08
 *
 */
//----------------------------------------------------------------------

#ifndef UR_CLIENT_LIBRARY_KINEMATICS_INFO_H_INCLUDED
#define UR_CLIENT_LIBRARY_KINEMATICS_INFO_H_INCLUDED

#include "ur_client_library/types.h"
#include "ur_client_library/primary/robot_state.h"
namespace urcl
{
namespace primary_interface
{
/*!
 * \brief This messages contains information about the robot's calibration. The DH parameters are
 * a combination between the perfect model parameters and the correction deltas as noted in the
 * configuration files on the robot controller.
 */
class KinematicsInfo : public RobotState
{
public:
  KinematicsInfo() : RobotState(RobotStateType::KINEMATICS_INFO)
  {
  }
  /*!
   * \brief Creates a new KinematicsInfo object.
   *
   * \param type The type of RobotState message received
   */
  explicit KinematicsInfo(const RobotStateType type) : RobotState(type)
  {
  }

  KinematicsInfo(const KinematicsInfo& other) : RobotState(RobotStateType::KINEMATICS_INFO)
  {
    checksum_ = other.checksum_;
    dh_theta_ = other.dh_theta_;
    dh_a_ = other.dh_a_;
    dh_d_ = other.dh_d_;
    dh_alpha_ = other.dh_alpha_;
    calibration_status_ = other.calibration_status_;
  }
  virtual ~KinematicsInfo() = default;

  /*!
   * \brief Sets the attributes of the package by parsing a serialized representation of the
   * package.
   *
   * \param bp A parser containing a serialized version of the package
   *
   * \returns True, if the package was parsed successfully, false otherwise
   */
  virtual bool parseWith(comm::BinParser& bp);

  /*!
   * \brief Consume this specific package with a specific consumer.
   *
   * \param consumer Placeholder for the consumer calling this
   *
   * \returns true on success
   */
  virtual bool consumeWith(AbstractPrimaryConsumer& consumer);

  /*!
   * \brief Produces a human readable representation of the package object.
   *
   * \returns A string representing the object
   */
  virtual std::string toString() const;

  /*!
   * \brief Calculates a hash value of the parameters to allow for identification of a calibration.
   *
   * \returns A hash value of the parameters
   */
  std::string toHash() const;

  vector6uint32_t checksum_;
  vector6d_t dh_theta_;
  vector6d_t dh_a_;
  vector6d_t dh_d_;
  vector6d_t dh_alpha_;
  uint32_t calibration_status_;  // According to the docs this should be uint8_t, but then I have 3 bytes left.
};

}  // namespace primary_interface
}  // namespace urcl

#endif  // ifndef UR_CLIENT_LIBRARY_KINEMATICS_INFO_H_INCLUDED
