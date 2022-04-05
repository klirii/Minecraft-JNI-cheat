#include <Windows.h>
#include <jni.h>
#include <iostream>
using namespace std;

JNIEnv* env;

jobject mc;
jobject thePlayer;
jobject capabilities;

/*
* Все названия классов, функций (методов) и переменных (полей) майнкрафта нужно указывать в зашифрованном виде, т.к они зашифрованы.
* Проверить это легко - достаточно открыть джарник игры через JD-GUI.
*/

void getEnv() { // Функция для получения указателя на JNIEnv
	JavaVM* jvm;

	typedef jint(*pJNI_GetCreatedJavaVMs)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs); // Создаём прототип функции JNI_GetCreatedJavaVMs
	pJNI_GetCreatedJavaVMs JNI_GetCreatedJavaVMs = reinterpret_cast<pJNI_GetCreatedJavaVMs>(GetProcAddress(GetModuleHandleA("jvm.dll"), "JNI_GetCreatedJavaVMs"));
	// Присваиваем прототипу этой функции адрес этой функции в игре

	JNI_GetCreatedJavaVMs(&jvm, 1, NULL); // Вызываем эту функцию
	cout << "[DEBUG] JVM*: " << (LPVOID)jvm << endl;
	jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), NULL); // Получаем указатель на JNIEnv
}

jobject getMC() { // Функция для получения объекта класса Minecraft (theMinecraft)
	jclass Minecraft = env->FindClass("ave"); // Ищем класс "Minecraft" в зашифрованном виде
	cout << "[DEBUG] Minecraft: " << Minecraft << endl;
	jmethodID smid = env->GetStaticMethodID(Minecraft, "A", "()Lave;"); // Получаем адрес функции (метода), которая возвращает объект класса Minecraft
	cout << "[DEBUG] public static Minecraft getMinecraft(): " << smid << endl;
	return env->CallStaticObjectMethod(Minecraft, smid); // Вызываем функцию getMinecraft() из игры, и возвращаем в качестве результата нашей функции результат getMinecraft()
}

/*
* 3-й аргумент функции GetStaticMethodID() - подпись метода, где
* () - указывают на то, что это функция (то, что идёт после скобочек - тип возвращаемого значения функции)
* L - нужен, если функция возвращает объект класса, этот символ обозначает то, что после него должно идти полное название класса, объект которого мы возвращаем
* Полное название класса - название класса вместе с пакетом
* Например: Lnet/minecraft/client/Minecraft;
* ; - обозначает окончание полного названия класса
* Подробнее - https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/types.html
*/

jobject getPlayer() { // Функция для получения объекта класса EntityPlayerSP (thePlayer)
	jfieldID fid = env->GetFieldID(env->GetObjectClass(mc), "h", "Lbew;"); // Получаем адрес объекта thePlayer (адрес поля в классе Minecraft)
	cout << "[DEBUG] public EntityPlayerSP thePlayer: " << fid << endl;
	return env->GetObjectField(mc, fid); // Получаем сам объект, и возвращаем его в качестве результата функции
}

jobject getCapabilities() { // Функция для получения объекта класса EntityPlayer (capabilities)
	jclass EntityPlayer = env->FindClass("wn"); // Ищем класс EntityPlayer в зашифрованном виде
	cout << "[DEBUG] EntityPlayer: " << EntityPlayer << endl;
	jfieldID fid = env->GetFieldID(EntityPlayer, "bA", "Lwl;"); // Получаем адрес объекта capabilities (адрес поля в классе EntityPlayer)
	cout << "[DEBUG] public PlayerCapabilities capabilities: " << fid << endl;
	return env->GetObjectField(thePlayer, fid); // Получаем сам объект capabilities
}

void initObjects() { // Функция для инициализации объектов, через которые мы будем работать с полями и методами игры
	mc = getMC();
	thePlayer = getPlayer();
	capabilities = getCapabilities();
}

void Fly() {
	jfieldID fid = env->GetFieldID(env->GetObjectClass(capabilities), "c", "Z"); // Получаем адрес поля allowFlying
	cout << "[DEBUG] public boolean allowFlying: " << fid << endl;
	env->SetBooleanField(capabilities, fid, true); // Устанавливаем переменную allowFlying на true
}

float getFlySpeed() {
	jmethodID mid = env->GetMethodID(env->GetObjectClass(capabilities), "a", "()F"); // Получаем адрес метода getFlySpeed
	return env->CallFloatMethod(capabilities, mid); // Вызываем функцию и получаем скорость флая
}

void setFlySpeed(float speed) {
	jmethodID mid = env->GetMethodID(env->GetObjectClass(capabilities), "a", "(F)V"); // Получаем адрес метода setFlySpeed
	env->CallVoidMethod(capabilities, mid, 0.05 * speed); // Вызываем его и устанавливаем скорость флая
}

BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		AllocConsole(); // Выделяем консоль
		freopen("CONOUT$", "w", stdout); // Выделяем поток stdout для консоли

		getEnv(); // Получаем JNIEnv*
		cout << "[DEBUG] JNIEnv*: " << (LPVOID)env << endl;
		initObjects(); // Инициализируем объекты для работы

		Fly(); // Делаем fly
		setFlySpeed(2);
		cout << "[DEBUG] Current fly speed: " << getFlySpeed() << endl;
	}
}