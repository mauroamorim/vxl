# prip/vmap/CMakeLists.txt

SET(vmap_sources

    # vbl extensions

   vbl_controlled_partition.cxx  vbl_controlled_partition.h
   vmap_map_iterator_wrapper.h
   vmap_ptr_sequences.h

    # types

   vmap_types.h
   vmap_kernel.txx               vmap_kernel.h
   vmap_oriented_kernel.txx      vmap_oriented_kernel.h
   vmap_non_oriented_kernel.txx  vmap_non_oriented_kernel.h
   vmap_grid_structure.cxx       vmap_grid_structure.h
   vmap_set_grid_structure.h

    # 2-map

   vmap_2_map.cxx                vmap_2_map.txx            vmap_2_map.h

    # 2-topological map

   vmap_2_tmap.txx               vmap_2_tmap.h
   vmap_2_tmap_elts.cxx          vmap_2_tmap_elts.h
)

ADD_LIBRARY(vmap ${vmap_sources})
INSTALL_TARGETS(/lib vmap)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/prip/vmap ${vmap_sources})
TARGET_LINK_LIBRARIES(vmap vcl)

#IF (BUILD_TESTING)
  SUBDIRS(tests)
#ENDIF (BUILD_TESTING)
