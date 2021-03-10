# How To Run:
* cmd 1 : g++ code.cpp
* cmd 2 : ./a.out
	* Terminal would ask for Pressing 0 or 1:
    	* 0 => For Extendible Hashing
     	* 1 => For Data Generation

### Before Running the code you may have to change the following varibles as per your requirements
* Line 4: SEC_MEM_SIZE => Memory Size of Secondary Memory (Default = 1000000)
* Line 5: BITS => Bits representation of a number to extract hash key (Default = 16)
* Line 6: MM_ENTRY => Number of Directory Entries (Default = 1024)

# Notes:
>    To test the code on smaller number of Records (near 16) please change "MM_ENTRY" at "Line 6" and "BITS" at "Line 5" to smaller value.
>
>    Example => MM_ENTRY = 4 and BITS = 4 (if Number of Records = 16)

# Info :
* I have create a Bucket_ADT class as a parent to Bucket and Directory classes.
* I have used RunTime Polymorphism(With function overriding) to instantiate Bucket and Directory for different position
* for Bucket_ADT of the Simulated Secondary Memory.
* I have considered first (1/2 of SSM for Data Buckets) and then (1/4 for overflow Buckets) and (Last 1/4 for Directory overflow Buckets).
* IMP: To find hash key value I have considered MSB(Most Significant bits) in binary representation. To show the actual simulation 
* I have converted the transactionId to binary and the extract the key value based on global debth. 
* To have real visualization I have searched for key in directory Linearly.


