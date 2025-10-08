The Source separation(SS SDK) provides a complete C-based Visual Studio framework for the Windows environment.

=========================================================================================================================
Visual Studio Recommended Versions:Visual Studio 2019
Supported Platforms:Windows 10 or Windows 11 (64-bit)

=========================================================================================================================
                  Steps to Run or Build Application on Windows via Visual Studio
=========================================================================================================================
1. Open the Project in Visual Studio
   - Navigate to:
     app\win\ss
   - Open the solution file:
     ss.sln
   - Set the Build Configuration to Debug or Release as needed
	(Set Release mode for faster execution)
   - Set command Arguments as
    <ipfile> <opfile> <op-sr> <fr-ms> <boardclock in Mhz>
	 Where:
     - <ipfile> → Input audio file  
     - <opfile> → Output processed audio file  
     - <op-sr>  → Output sampling rate 
     - <fr-ms>  → Frame size in milliseconds (e.g., 10)  
     - <boardclock in Mhz> → Clock frequency of the board 
	ex: ss.exe input.wav output.wav 16000 10 2200
	
   
3.Supported Framesizes in ms: 10 
4.Supported Smapling rates in KHz: 16000

==========================================================================================================================
                                    Folder Structure
==========================================================================================================================
+---app
|   \---win
|       \---ss
|               ss.sln
|               ss.vcxproj
|               ss.vcxproj.filters
|               ss.vcxproj.user
|
+---ss
|       mt_cv_ss_api.c
|       mt_cv_ss_api.h
|       mt_cv_ss_defines.h
|       mt_cv_ss_loc.h
|       mt_cv_ss_proc.c
|       mt_cv_ss_resep.c
|       mt_cv_ss_resep_avx.c
|       mt_cv_ss_struct.h
|       mt_cv_ss_tbl.c
|       mt_cv_ss_tbl.h
|
+---testvectors
|       ip.wav
|       op.wav
|
\---test_wrapper
        mt_cv_ss_test.c
        


==================================================================================================
       Memory Details      
==================================================================================================
SDK memory - ~40 MB
==================================================================================================

==================================================================================================
        CPU Details@ 
Intel(R) Core(TM) i7-10870H CPU 
@ 2.20GHz, 2208 MHz, 8 Core(s),
16 Logical Processor(s)
==================================================================================================
| Support |  DNN(MCPS)  |
|---------|------------ |
| C       | 400 GHz     |
| AVX2    | 100 GHz     |
==================================================================================================

==================================================================================================
 Additional Information   
==================================================================================================
| Algorithm Delay  |  20 ms  |
==================================================================================================

