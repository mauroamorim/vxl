// This is core/vul/tests/test_debug.cxx

//:
// \file
// \brief Tests core dumping etc.
// \author Ian Scott
//

#include <vcl_iostream.h>
#include <vcl_new.h>
#include <vul/vul_debug.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>


#include <testlib/testlib_test.h>


//=======================================================================
void test_debug()
{
  vcl_cout <<"\n*******************\n"
           <<  " Testing vul_debug\n"
           <<  "*******************\n\n";

  
  {
    vcl_cout << "Test simple forced coredump\n";

    const char * filename = "test_core.dmp";
    vpl_unlink(filename);

    vul_debug_core_dump(filename);

    TEST("Core dump file exists", vul_file_exists(filename), true);
    TEST("Core dump file is sensible size", vul_file_size(filename) > 100, true);
  }


#ifdef _WIN32
  {
    vcl_cout << "Test Structured exception coredump\n";

    const char * filename = "test_core2.dmp";
    vpl_unlink(filename);

    vul_debug_set_coredump_and_throw_on_windows_se(filename);
    bool caught_exception=false;
    try
    {
      // force an segmentation violation exception
      vcl_cout << *static_cast<int *>(0);
    }
    catch (const vul_debug_windows_structured_exception &)
    {
      caught_exception=true;
    }
    TEST("Exception caught", caught_exception, true);
    TEST("Core dump file exists", vul_file_exists(filename), true);
    TEST("Core dump file is sensible size", vul_file_size(filename) > 100, true);
  }
#endif

#ifdef VCL_HAS_EXCEPTIONS
  {
    vcl_cout << "Test out-out-memory coredump\n";

    const char * filename = "test_core3.dmp";
    vpl_unlink(filename);

    vul_debug_set_coredump_and_throw_on_out_of_memory(filename);
    bool caught_exception=false;
    try
    {
      char * p = new char[vcl_size_t(-1000)];
    }
    catch (const vcl_bad_alloc &)
    {
      caught_exception=true;
    }
    TEST("Exception caught", caught_exception, true);
    TEST("Core dump file exists", vul_file_exists(filename), true);
    TEST("Core dump file is sensible size", vul_file_size(filename) > 100, true);
  }
#endif
}

TESTMAIN(test_debug);
