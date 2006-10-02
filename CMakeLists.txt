#Change PROJECT_NAME to the name of your project
PROJECT(fastRankMean)

# set the name of the input image used to run the test
SET(INPUT_IMAGE ${CMAKE_SOURCE_DIR}/images/cthead1.png)
SET(INPUT_MASK ${CMAKE_SOURCE_DIR}/images/mask.png)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)
FIND_PROGRAM(IMAGE_COMPARE ImageCompare)

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
  ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  ITKCommon
  ITKBasicFilters
  ITKIO
)


# install devel files
OPTION(INSTALL_DEVEL_FILES "Install C++ headers" ON)
IF(INSTALL_DEVEL_FILES)
FILE(GLOB develFiles *.h *.txx) 
FOREACH(f ${develFiles})
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${f})
ENDFOREACH(f)
ENDIF(INSTALL_DEVEL_FILES)



# option for wrapping
OPTION(BUILD_WRAPPERS "Wrap library" OFF)
IF(BUILD_WRAPPERS)
   SUBDIRS(Wrapping)
ENDIF(BUILD_WRAPPERS)
   
   

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project

IF(BUILD_TESTING)

FOREACH(CurrentExe "test2DCharHistMedian" "test2DShortHistMedian" "test2DIntHistMedian" "perfMedian")

ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

ENDFOREACH(CurrentExe)

FOREACH(CurrentExe "test2DCharHistMean" "test2DShortHistMean" "test2DIntHistMean")

ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

ENDFOREACH(CurrentExe)

FOREACH(CurrentExe "test2DCharHistMedianMask" "test2DIntHistMedianMask")

ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

ENDFOREACH(CurrentExe)
FOREACH(CurrentExe "test2DSepMedian" "test2DSepMaskMedian")

ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

ENDFOREACH(CurrentExe)

ENDIF(BUILD_TESTING)

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
# ADD_TEST(Testname ExecutableToRun arg1 arg2 arg3)

#ADD_TEST(Run check ${INPUT_IMAGE} out.png)
#ADD_TEST(CompareImage ${IMAGE_COMPARE} out.png ${CMAKE_SOURCE_DIR}/images/test.png)

ADD_TEST(test2Dchar test2DCharHistMedian 1 ${INPUT_IMAGE} chr_hist.png chr_std.png)
ADD_TEST(test2Dshort test2DShortHistMedian 1 ${INPUT_IMAGE} shrt_hist.nii chr_std.nii)
ADD_TEST(test2Dint test2DCharHistMedian 1 ${INPUT_IMAGE} int_hist.nii chr_std.nii)

ADD_TEST(compChrShrt ${IMAGE_COMPARE} chr_hist.png shrt_hist.nii)
ADD_TEST(compChrInt ${IMAGE_COMPARE} chr_hist.png int_hist.nii)
ADD_TEST(compShrtInt ${IMAGE_COMPARE} shrt_hist.nii int_hist.nii)

ADD_TEST(test2Dchar_mean test2DCharHistMean 1 ${INPUT_IMAGE} chr_hist_mean.png chr_std_mean.png)
ADD_TEST(test2Dshort_mean test2DShortHistMean 1 ${INPUT_IMAGE} shrt_hist_mean.nii chr_std_mean.nii)
ADD_TEST(test2Dint_mean test2DCharHistMean 1 ${INPUT_IMAGE} int_hist_mean.nii chr_std_mean.nii)

ADD_TEST(compMeanChrShrt ${IMAGE_COMPARE} chr_hist_mean.png shrt_hist_mean.nii)
ADD_TEST(compMeanChrInt ${IMAGE_COMPARE} chr_hist_mean.png int_hist_mean.nii)
ADD_TEST(compMeanShrtInt ${IMAGE_COMPARE} shrt_hist_mean.nii int_hist_mean.nii)

ADD_TEST(test2Dchar_mask_med test2DCharHistMedianMask 1 ${INPUT_IMAGE} ${INPUT_MASK} chr_mask_med.png )
ADD_TEST(test2Dint_mask_med test2DIntHistMedianMask 1 ${INPUT_IMAGE} ${INPUT_MASK} int_mask_med.nii )
ADD_TEST(compMaskMedChrInt ${IMAGE_COMPARE} chr_mask_med.png int_mask_med.nii)
