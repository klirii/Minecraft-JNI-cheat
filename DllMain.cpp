#include <Windows.h>
#include <jni.h>
#include <iostream>
using namespace std;

JNIEnv* env;

jobject mc;
jobject thePlayer;
jobject capabilities;

/*
* ��� �������� �������, ������� (�������) � ���������� (�����) ���������� ����� ��������� � ������������� ����, �.� ��� �����������.
* ��������� ��� ����� - ���������� ������� ������� ���� ����� JD-GUI.
*/

void getEnv() { // ������� ��� ��������� ��������� �� JNIEnv
	JavaVM* jvm;

	typedef jint(*pJNI_GetCreatedJavaVMs)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs); // ������ �������� ������� JNI_GetCreatedJavaVMs
	pJNI_GetCreatedJavaVMs JNI_GetCreatedJavaVMs = reinterpret_cast<pJNI_GetCreatedJavaVMs>(GetProcAddress(GetModuleHandleA("jvm.dll"), "JNI_GetCreatedJavaVMs"));
	// ����������� ��������� ���� ������� ����� ���� ������� � ����

	JNI_GetCreatedJavaVMs(&jvm, 1, NULL); // �������� ��� �������
	cout << "[DEBUG] JVM*: " << (LPVOID)jvm << endl;
	jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), NULL); // �������� ��������� �� JNIEnv
}

jobject getMC() { // ������� ��� ��������� ������� ������ Minecraft (theMinecraft)
	jclass Minecraft = env->FindClass("ave"); // ���� ����� "Minecraft" � ������������� ����
	cout << "[DEBUG] Minecraft: " << Minecraft << endl;
	jmethodID smid = env->GetStaticMethodID(Minecraft, "A", "()Lave;"); // �������� ����� ������� (������), ������� ���������� ������ ������ Minecraft
	cout << "[DEBUG] public static Minecraft getMinecraft(): " << smid << endl;
	return env->CallStaticObjectMethod(Minecraft, smid); // �������� ������� getMinecraft() �� ����, � ���������� � �������� ���������� ����� ������� ��������� getMinecraft()
}

/*
* 3-� �������� ������� GetStaticMethodID() - ������� ������, ���
* () - ��������� �� ��, ��� ��� ������� (��, ��� ��� ����� �������� - ��� ������������� �������� �������)
* L - �����, ���� ������� ���������� ������ ������, ���� ������ ���������� ��, ��� ����� ���� ������ ���� ������ �������� ������, ������ �������� �� ����������
* ������ �������� ������ - �������� ������ ������ � �������
* ��������: Lnet/minecraft/client/Minecraft;
* ; - ���������� ��������� ������� �������� ������
* ��������� - https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/types.html
*/

jobject getPlayer() { // ������� ��� ��������� ������� ������ EntityPlayerSP (thePlayer)
	jfieldID fid = env->GetFieldID(env->GetObjectClass(mc), "h", "Lbew;"); // �������� ����� ������� thePlayer (����� ���� � ������ Minecraft)
	cout << "[DEBUG] public EntityPlayerSP thePlayer: " << fid << endl;
	return env->GetObjectField(mc, fid); // �������� ��� ������, � ���������� ��� � �������� ���������� �������
}

jobject getCapabilities() { // ������� ��� ��������� ������� ������ EntityPlayer (capabilities)
	jclass EntityPlayer = env->FindClass("wn"); // ���� ����� EntityPlayer � ������������� ����
	cout << "[DEBUG] EntityPlayer: " << EntityPlayer << endl;
	jfieldID fid = env->GetFieldID(EntityPlayer, "bA", "Lwl;"); // �������� ����� ������� capabilities (����� ���� � ������ EntityPlayer)
	cout << "[DEBUG] public PlayerCapabilities capabilities: " << fid << endl;
	return env->GetObjectField(thePlayer, fid); // �������� ��� ������ capabilities
}

void initObjects() { // ������� ��� ������������� ��������, ����� ������� �� ����� �������� � ������ � �������� ����
	mc = getMC();
	thePlayer = getPlayer();
	capabilities = getCapabilities();
}

void Fly() {
	jfieldID fid = env->GetFieldID(env->GetObjectClass(capabilities), "c", "Z"); // �������� ����� ���� allowFlying
	cout << "[DEBUG] public boolean allowFlying: " << fid << endl;
	env->SetBooleanField(capabilities, fid, true); // ������������� ���������� allowFlying �� true
}

float getFlySpeed() {
	jmethodID mid = env->GetMethodID(env->GetObjectClass(capabilities), "a", "()F"); // �������� ����� ������ getFlySpeed
	return env->CallFloatMethod(capabilities, mid); // �������� ������� � �������� �������� ����
}

void setFlySpeed(float speed) {
	jmethodID mid = env->GetMethodID(env->GetObjectClass(capabilities), "a", "(F)V"); // �������� ����� ������ setFlySpeed
	env->CallVoidMethod(capabilities, mid, 0.05 * speed); // �������� ��� � ������������� �������� ����
}

BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		AllocConsole(); // �������� �������
		freopen("CONOUT$", "w", stdout); // �������� ����� stdout ��� �������

		getEnv(); // �������� JNIEnv*
		cout << "[DEBUG] JNIEnv*: " << (LPVOID)env << endl;
		initObjects(); // �������������� ������� ��� ������

		Fly(); // ������ fly
		setFlySpeed(2);
		cout << "[DEBUG] Current fly speed: " << getFlySpeed() << endl;
	}
}