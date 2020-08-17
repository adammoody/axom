// wrapIOManager.cpp
// This is generated code, do not edit
//
// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
#include "wrapIOManager.h"
#include <cstddef>
#include <string>
#include "axom/sidre/spio/IOManager.hpp"

// splicer begin class.IOManager.CXX_definitions
// splicer end class.IOManager.CXX_definitions

extern "C" {

// splicer begin class.IOManager.C_definitions
// splicer end class.IOManager.C_definitions

SPIO_IOManager* SPIO_IOManager_new_0(MPI_Fint com, SPIO_IOManager* SHC_rv)
{
  // splicer begin class.IOManager.method.new_0
  MPI_Comm SHCXX_com = MPI_Comm_f2c(com);
  axom::sidre::IOManager* SHCXX_rv = new axom::sidre::IOManager(SHCXX_com);
  SHC_rv->addr = static_cast<void*>(SHCXX_rv);
  SHC_rv->idtor = 0;
  return SHC_rv;
  // splicer end class.IOManager.method.new_0
}

SPIO_IOManager* SPIO_IOManager_new_1(MPI_Fint com, bool use_scr,
                                     SPIO_IOManager* SHC_rv)
{
  // splicer begin class.IOManager.method.new_1
  MPI_Comm SHCXX_com = MPI_Comm_f2c(com);
  axom::sidre::IOManager* SHCXX_rv = new axom::sidre::IOManager(SHCXX_com,
                                                                use_scr);
  SHC_rv->addr = static_cast<void*>(SHCXX_rv);
  SHC_rv->idtor = 0;
  return SHC_rv;
  // splicer end class.IOManager.method.new_1
}

void SPIO_IOManager_delete(SPIO_IOManager* self)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.delete
  delete SH_this;
  self->addr = NULL;
  return;
  // splicer end class.IOManager.method.delete
}

void SPIO_IOManager_write(SPIO_IOManager* self, SIDRE_Group* group,
                          int num_files, const char* file_string,
                          const char* protocol)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.write
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_string(file_string);
  const std::string SHCXX_protocol(protocol);
  SH_this->write(SHCXX_group, num_files, SHCXX_file_string, SHCXX_protocol);
  return;
  // splicer end class.IOManager.method.write
}

void SPIO_IOManager_write_bufferify(SPIO_IOManager* self, SIDRE_Group* group,
                                    int num_files, const char* file_string,
                                    int Lfile_string, const char* protocol,
                                    int Lprotocol)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.write_bufferify
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_string(file_string, Lfile_string);
  const std::string SHCXX_protocol(protocol, Lprotocol);
  SH_this->write(SHCXX_group, num_files, SHCXX_file_string, SHCXX_protocol);
  return;
  // splicer end class.IOManager.method.write_bufferify
}

void SPIO_IOManager_write_group_to_root_file(SPIO_IOManager* self,
                                             SIDRE_Group* group,
                                             const char* file_name)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.write_group_to_root_file
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_name(file_name);
  SH_this->writeGroupToRootFile(SHCXX_group, SHCXX_file_name);
  return;
  // splicer end class.IOManager.method.write_group_to_root_file
}

void SPIO_IOManager_write_group_to_root_file_bufferify(SPIO_IOManager* self,
                                                       SIDRE_Group* group,
                                                       const char* file_name,
                                                       int Lfile_name)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.write_group_to_root_file_bufferify
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_name(file_name, Lfile_name);
  SH_this->writeGroupToRootFile(SHCXX_group, SHCXX_file_name);
  return;
  // splicer end class.IOManager.method.write_group_to_root_file_bufferify
}

void SPIO_IOManager_write_blueprint_index_to_root_file(SPIO_IOManager* self,
                                                       SIDRE_DataStore* datastore, const char* domain_path, const char* file_name,
                                                       const char* mesh_path)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.write_blueprint_index_to_root_file
  axom::sidre::DataStore* SHCXX_datastore =
    static_cast<axom::sidre::DataStore*>(datastore->addr);
  const std::string SHCXX_domain_path(domain_path);
  const std::string SHCXX_file_name(file_name);
  const std::string SHCXX_mesh_path(mesh_path);
  SH_this->writeBlueprintIndexToRootFile(SHCXX_datastore, SHCXX_domain_path,
                                         SHCXX_file_name, SHCXX_mesh_path);
  return;
  // splicer end class.IOManager.method.write_blueprint_index_to_root_file
}

void SPIO_IOManager_write_blueprint_index_to_root_file_bufferify(
  SPIO_IOManager* self, SIDRE_DataStore* datastore, const char* domain_path,
  int Ldomain_path, const char* file_name, int Lfile_name,
  const char* mesh_path, int Lmesh_path)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin
  // class.IOManager.method.write_blueprint_index_to_root_file_bufferify
  axom::sidre::DataStore* SHCXX_datastore =
    static_cast<axom::sidre::DataStore*>(datastore->addr);
  const std::string SHCXX_domain_path(domain_path, Ldomain_path);
  const std::string SHCXX_file_name(file_name, Lfile_name);
  const std::string SHCXX_mesh_path(mesh_path, Lmesh_path);
  SH_this->writeBlueprintIndexToRootFile(SHCXX_datastore, SHCXX_domain_path,
                                         SHCXX_file_name, SHCXX_mesh_path);
  return;
  // splicer end
  // class.IOManager.method.write_blueprint_index_to_root_file_bufferify
}

void SPIO_IOManager_read_0(SPIO_IOManager* self, SIDRE_Group* group,
                           const char* file_string, const char* protocol)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_0
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_string(file_string);
  const std::string SHCXX_protocol(protocol);
  SH_this->read(SHCXX_group, SHCXX_file_string, SHCXX_protocol);
  return;
  // splicer end class.IOManager.method.read_0
}

void SPIO_IOManager_read_0_bufferify(SPIO_IOManager* self, SIDRE_Group* group,
                                     const char* file_string, int Lfile_string,
                                     const char* protocol, int Lprotocol)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_0_bufferify
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_string(file_string, Lfile_string);
  const std::string SHCXX_protocol(protocol, Lprotocol);
  SH_this->read(SHCXX_group, SHCXX_file_string, SHCXX_protocol);
  return;
  // splicer end class.IOManager.method.read_0_bufferify
}

void SPIO_IOManager_read_1(SPIO_IOManager* self, SIDRE_Group* group,
                           const char* file_string, const char* protocol,
                           bool preserve_contents)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_1
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_string(file_string);
  const std::string SHCXX_protocol(protocol);
  SH_this->read(SHCXX_group, SHCXX_file_string, SHCXX_protocol,
                preserve_contents);
  return;
  // splicer end class.IOManager.method.read_1
}

void SPIO_IOManager_read_1_bufferify(SPIO_IOManager* self, SIDRE_Group* group,
                                     const char* file_string, int Lfile_string,
                                     const char* protocol, int Lprotocol,
                                     bool preserve_contents)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_1_bufferify
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_file_string(file_string, Lfile_string);
  const std::string SHCXX_protocol(protocol, Lprotocol);
  SH_this->read(SHCXX_group, SHCXX_file_string, SHCXX_protocol,
                preserve_contents);
  return;
  // splicer end class.IOManager.method.read_1_bufferify
}

void SPIO_IOManager_read_2(SPIO_IOManager* self, SIDRE_Group* group,
                           const char* root_file)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_2
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_root_file(root_file);
  SH_this->read(SHCXX_group, SHCXX_root_file);
  return;
  // splicer end class.IOManager.method.read_2
}

void SPIO_IOManager_read_2_bufferify(SPIO_IOManager* self, SIDRE_Group* group,
                                     const char* root_file, int Lroot_file)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_2_bufferify
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_root_file(root_file, Lroot_file);
  SH_this->read(SHCXX_group, SHCXX_root_file);
  return;
  // splicer end class.IOManager.method.read_2_bufferify
}

void SPIO_IOManager_read_3(SPIO_IOManager* self, SIDRE_Group* group,
                           const char* root_file, bool preserve_contents)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_3
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_root_file(root_file);
  SH_this->read(SHCXX_group, SHCXX_root_file, preserve_contents);
  return;
  // splicer end class.IOManager.method.read_3
}

void SPIO_IOManager_read_3_bufferify(SPIO_IOManager* self, SIDRE_Group* group,
                                     const char* root_file, int Lroot_file,
                                     bool preserve_contents)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.read_3_bufferify
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_root_file(root_file, Lroot_file);
  SH_this->read(SHCXX_group, SHCXX_root_file, preserve_contents);
  return;
  // splicer end class.IOManager.method.read_3_bufferify
}

void SPIO_IOManager_load_external_data(SPIO_IOManager* self, SIDRE_Group* group,
                                       const char* root_file)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.load_external_data
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_root_file(root_file);
  SH_this->loadExternalData(SHCXX_group, SHCXX_root_file);
  return;
  // splicer end class.IOManager.method.load_external_data
}

void SPIO_IOManager_load_external_data_bufferify(SPIO_IOManager* self,
                                                 SIDRE_Group* group,
                                                 const char* root_file,
                                                 int Lroot_file)
{
  axom::sidre::IOManager* SH_this =
    static_cast<axom::sidre::IOManager*>(self->addr);
  // splicer begin class.IOManager.method.load_external_data_bufferify
  axom::sidre::Group* SHCXX_group =
    static_cast<axom::sidre::Group*>(group->addr);
  const std::string SHCXX_root_file(root_file, Lroot_file);
  SH_this->loadExternalData(SHCXX_group, SHCXX_root_file);
  return;
  // splicer end class.IOManager.method.load_external_data_bufferify
}

}  // extern "C"
