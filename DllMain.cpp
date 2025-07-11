#include <Windows.h>
#include <jni.h>
#include <iostream>
using namespace std;

JNIEnv* env;

jobject mc;
jobject thePlayer;
jobject capabilities;

/*
* All names of classes, methods and fields of minecraft should be specified in encrypted form, because they are encrypted.
* For clarity you can open the game jar file in the program JD-GUI (https://java-decompiler.github.io/).
*/

void getEnv() { // Function to get a pointer to JNIEnv
	JavaVM* jvm;

	typedef jint(*pJNI_GetCreatedJavaVMs)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs); // Create a prototype of the JNI_GetCreatedJavaVMs function
	pJNI_GetCreatedJavaVMs JNI_GetCreatedJavaVMs = reinterpret_cast<pJNI_GetCreatedJavaVMs>(GetProcAddress(GetModuleHandleA("jvm.dll"), "JNI_GetCreatedJavaVMs"));
	// Assign the address of this function in the game to the prototype

	JNI_GetCreatedJavaVMs(&jvm, 1, NULL); // Call this function
	cout << "[DEBUG] JVM*: " << (LPVOID)jvm << endl;
	jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), NULL); // Get a pointer to JNIEnv
}

jobject getMC() { // Function to get an object of the Minecraft class (theMinecraft)
	jclass Minecraft = env->FindClass("ave"); // Looking for the class “Minecraft” in encrypted form
	cout << "[DEBUG] Minecraft: " << Minecraft << endl;
	jmethodID smid = env->GetStaticMethodID(Minecraft, "A", "()Lave;"); // Get the handle of the method that returns an object of the Minecraft class
	cout << "[DEBUG] public static Minecraft getMinecraft(): " << smid << endl;
	return env->CallStaticObjectMethod(Minecraft, smid); // Call the getMinecraft() method from the game, and return the result of our function as the result of getMinecraft()
}

/*
* The 3rd argument of the GetStaticMethodID() is the method signature, where:
* () - indicate that this is a method (what comes after the brackets is the type of the method's return value)
* L - is needed if the method returns an object, this symbol indicates that it should be followed by the full name of the class whose object we return
* Full class name = the name of the class along with the package
* For example: Lnet/minecraft/client/Minecraft;
* ; - indicates the ending of the full name of the class
* Read more - https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/types.html
*/

jobject getPlayer() { // Function to get an object of EntityPlayerSP class (thePlayer)
	jfieldID fid = env->GetFieldID(env->GetObjectClass(mc), "h", "Lbew;"); // Get the handle of thePlayer object
	cout << "[DEBUG] public EntityPlayerSP thePlayer: " << fid << endl;
	return env->GetObjectField(mc, fid); // Get the object and return it as the result of the function
}

jobject getCapabilities() { // Function to get an object of EntityPlayer class (capabilities)
	jclass EntityPlayer = env->FindClass("wn"); // Looking for EntityPlayer class in encrypted form
	cout << "[DEBUG] EntityPlayer: " << EntityPlayer << endl;
	jfieldID fid = env->GetFieldID(EntityPlayer, "bA", "Lwl;"); // Get the handle of the capabilities object
	cout << "[DEBUG] public PlayerCapabilities capabilities: " << fid << endl;
	return env->GetObjectField(thePlayer, fid); // Get an object and return it
}

void initObjects() { // Function for initialization of objects through which we will work with the fields and methods of the game
	mc = getMC();
	thePlayer = getPlayer();
	capabilities = getCapabilities();
}

void Fly() {
	jfieldID fid = env->GetFieldID(env->GetObjectClass(capabilities), "c", "Z"); // Get the handle of the allowFlying field
	cout << "[DEBUG] public boolean allowFlying: " << fid << endl;
	env->SetBooleanField(capabilities, fid, true); // Set the allowFlying field to true
}

float getFlySpeed() {
	jmethodID mid = env->GetMethodID(env->GetObjectClass(capabilities), "a", "()F"); // Get the handle of getFlySpeed method
	return env->CallFloatMethod(capabilities, mid); // Call the method and get the fly speed
}

void setFlySpeed(float speed) {
	jmethodID mid = env->GetMethodID(env->GetObjectClass(capabilities), "a", "(F)V"); // Get the handle of the setFlySpeed method
	env->CallVoidMethod(capabilities, mid, 0.05 * speed); // Call it and set the fly speed.
}

BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		AllocConsole(); // Allocate console
		freopen("CONOUT$", "w", stdout); // Allocate the stdout stream for the console

		getEnv(); // Get JNIEnv*
		cout << "[DEBUG] JNIEnv*: " << (LPVOID)env << endl;
		initObjects(); // Initialize objects for work

		Fly(); // Make fly
		setFlySpeed(2);
		cout << "[DEBUG] Current fly speed: " << getFlySpeed() << endl;
	}
}
