We have built/Cross-Compiled "FreeRDP for ChromeOS" on Ubuntu Linux VM.
Please refer to the below instructions

1) Setup Build Environment & Dependencies
 a. Download Nacl SDK (Pepper 49)
    - Please refer to instrunctions from https://developer.chrome.com/native-client/sdk/download
	- Set the enviromnet variable for this path. 
	<example:> export FREERDP_NACL_SDK_ROOT="/home/mkumara/gonacl/nacl_sdk/pepper_49"
	           export FREERDP_NACL_OPENSSL_ROOT="/home/mkumara/gonacl/payload"
			   mkumara@mkumara-VirtualBox:~/gonacl/payload$ pwd
				/home/mkumara/gonacl/payload
				mkumara@mkumara-VirtualBox:~/gonacl/payload$ ls
				bin  include  lib  ssl

 b. Download Nacl OpenSSl library Binary    
    - find the tar at the following URL	http://gsdview.appspot.com/naclports/builds/pepper_49/
	- please donwload the specified version only due to compatibility issues: trunk-740-g17a59c3/
	- extract the tar file to an accessible location
	- Set the enviromnet variable for this path. 
	<example:>  export FREERDP_NACL_OPENSSL_ROOT="/home/mkumara/gonacl/payload"
		   mkumara@mkumara-VirtualBox:~/gonacl/payload$ pwd
			/home/mkumara/gonacl/payload
			mkumara@mkumara-VirtualBox:~/gonacl/payload$ ls
			bin  include  lib  ssl
 
2. Download the FreeRDP code:
    <example:> git clone https://github.com/cmsaravana/FreeRDP.git forkrepo07
	
3. Goto the cloned repository Main directory
   <example:> cd forkrepo06	

4. switch to the freerdp_chromerdpplugin branch
	mkumara@mkumara-VirtualBox:~/github/forkrepo07$ git checkout freerdp_chromerdpplugin
	Branch freerdp_chromerdpplugin set up to track remote branch freerdp_chromerdpplugin from origin.
	Switched to a new branch 'freerdp_chromerdpplugin'
	mkumara@mkumara-VirtualBox:~/github/forkrepo07$ git branch
	* freerdp_chromerdpplugin
	  master	

5. Apply the attached Patch file located in Client/ChromeOs directory
	mkumara@mkumara-VirtualBox:~/github/forkrepo07$ git apply client/ChromeOs/freerdp_patch_06.diff
    
6. Execute the following cmake command to setup the build environment
	cmake . -DCMAKE_TOOLCHAIN_FILE=client/ChromeOs/ChromeOsToolchain.cmake  -DBUILD_SHARED_LIBS=OFF
	<example:> mkumara@mkumara-VirtualBox:~/github/forkrepo07$ cmake . -DCMAKE_TOOLCHAIN_FILE=client/ChromeOs/ChromeOsToolchain.cmake  -DBUILD_SHARED_LIBS=OFF
	
7. Execute make command to build freeRDP libraries.   
	mkumara@mkumara-VirtualBox:~/github/forkrepo07$ make
	Scanning dependencies of target winpr
	..
	
8. Execute make command to build the ChromeOS Plugin
    a. switch to client/ChromeOS directory
	b. Issue make command
	mkumara@mkumara-VirtualBox:~/github/forkrepo07/client/ChromeOs$ make
	make -C /home/mkumara/gonacl/nacl_sdk/pepper_49/src/ppapi_simple STAMPDIR=/home/mkumara/github/forkrepo07/client/ChromeOs/pnacl/Release /home/mkumara/github/forkrepo07/client/ChromeOs/pnacl/Release/ppapi_simple.stamp
	make[1]: Entering directory '/home/mkumara/gonacl/nacl_sdk/pepper_49/src/ppapi_simple'

9. the plug-in(.pexe) will be created inside the pnacl directory tree
	mkumara@mkumara-VirtualBox:~/github/forkrepo07/client/ChromeOs$ cd pnacl/Release/
	mkumara@mkumara-VirtualBox:~/github/forkrepo07/client/ChromeOs/pnacl/Release$ ls
	dir.stamp         graphics_2d.nmf  graphics_2d.pexe           graphics_2d_unstripped.pexe  ppapi_simple.stamp
	graphics_2d.deps  graphics_2d.o    graphics_2d_unstripped.bc  nacl_io.stamp
	mkumara@mkumara-VirtualBox:~/github/forkrepo07/client/ChromeOs/pnacl/Release$
