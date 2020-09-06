/*
Programme créé par Pierre TASSAN: pierre.tassan@etu.univ-st-etienne.fr
*/

#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <TlHelp32.h>
#include <String>
#include <filesystem>
#include <fstream>
#include "stdlib.h"

#pragma comment(lib, "Urlmon.lib")

using namespace std;



/*Récupérer l'adresse d'un module d'un logiciel ou d'un DLL d'un logiciel
Utilisé pour récupérer l'adresse du DLL gérant Cisco Webex
Remarque: On utilise une variable uintptr_t car celle-ci est de la taille d'un pointeur, nous pouvons donc stocker facilement l'adresse du DLL / du .exe
Plus d'infos: https://en.cppreference.com/w/cpp/types/integer
*/
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0; //initialisation de la variable pointer
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId); //Création de la snapshot du processus, plus d'infos https://docs.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot
	//L'objectif est d'obtenir un Handle, une sorte de pointeur vers la logiciel qui permet ensuite d'accéder à de nombreux paramètres de ce logiciel
	//Plus d'infos: https://en.wikipedia.org/wiki/Handle_(computing)
	if (hSnap != INVALID_HANDLE_VALUE) //Vérification de routine 
	{
		MODULEENTRY32 modEntry; //Variable pour le stockage des modules de l'application -> https://docs.microsoft.com/en-us/windows/win32/api/tlhelp32/ns-tlhelp32-moduleentry32
		modEntry.dwSize = sizeof(modEntry); //Choix de la taille
		if (Module32First(hSnap, &modEntry)) //Premier module attaché au programme
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName)) //Comparaison entre le module trouvé et le nom du module passé en paramètre (Pour qu'on ne modifie que le bvon) -> https://docs.microsoft.com/fr-fr/cpp/c-runtime-library/reference/stricmp-wcsicmp-mbsicmp-stricmp-l-wcsicmp-l-mbsicmp-l?view=vs-2019
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr; //On récupère son adrese
					break;
				}
			} while (Module32Next(hSnap, &modEntry)); //On continue la boucle tant que le break n'est pas déclenché pour tous les modules
		}
	}
	CloseHandle(hSnap); //On ferme le Handle précedemment ouvert (utilisé pour accéder au programme) 
	return modBaseAddr; //On retourne l'adresse
}

//Convertisseur string en wstring (wide string)
//Source: https://codereview.stackexchange.com/questions/419/converting-between-stdwstring-and-stdstring
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}


int main()
{
	CoInitialize(NULL);
	
	
	//Détermination du chemin de stockage des fichiers locaux
	char temppath[MAX_PATH + 20];
	GetTempPathA(sizeof(temppath), temppath);
	string temppath_string = temppath;
	temppath_string = temppath_string + "versions_muteme.txt";
	//Débug
	//cout << "Temppath: " << temppath_string << endl;
	wstring long_string_to_convert = s2ws(temppath_string); //Conversion du string en wstring pour permettre la future conversion en LPCWSTR
	LPCWSTR lien_temp_file = long_string_to_convert.c_str(); //On stock dans une variable LPCWSTR le chemin d'accès du fichier temporaire de stockage des versions
	//Pour plus d'info sur les LPCWSTR: https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/76f10dd8-699d-45e6-a53c-5aefc586da20

	//Détermination du chemin de stockage du fichier des versions en ligne
	//Génération de variables aléatoire pour éviter que le fichier lu en ligne soit un fichier local en Cache
	int random1 = rand() % 10 + 1;
	int random2 = rand() % 10 + 1;
	int random3 = rand() % 10 + 1;
	int random4 = rand() % 10 + 1;
	string urlpath_string = "http://154.49.211.209/muteme/versions.txt?fakevariable=" + random1 + random2 + random3 + random4; //Création du lien de téléchargement
	//On remarque l'ajout de variables aléatoires pour éviter le lecture d'un fichier local
	//Le serveur appartient à Pierre TASSAN
	//Débug
	//cout << "Temppath: " << temppath_string << endl;
	wstring long_string_to_convert_url = s2ws(urlpath_string); //Conversion du string en wstring pour permettre la future conversion en LPCWSTR
	LPCWSTR lien_temp_file_url = long_string_to_convert_url.c_str(); //On stock dans une variable LPCWSTR le chemin d'accès du fichier temporaire de stockage des versions
	//Pour plus d'info sur les LPCWSTR: https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/76f10dd8-699d-45e6-a53c-5aefc586da20

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); //On crée un Handle sur la console pour modifier la couleur
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED); //Couleur ROUGE
	cout << "Mute Me! by Pierre TASSAN @lesfreresdemon\n";
	cout << "Initialisation en cours, assurez vous d'avoir Cisco d'ouvert!\n";

	//Avertissement
	int reponse = MessageBox(NULL, (LPCUWSTR)L"Ce logiciel bloque la sortie du micro pour Cisco.\nAttention, il se déclenchera aussi si vous vous démutez volontairement de Cisco, pensez à appuyer sur F2 avant de parler.\n\nLogiciel créé par Pierre TASSAN en C++, ce logiciel ne doit PAS être utilisé avec Valorant ou CS:GO en arrière plan car ses méthodes sont similaires aux logiciels de triche!", (LPCUWSTR)L"Attention!", MB_ICONEXCLAMATION | MB_OKCANCEL);

	if (reponse == 2) {
		exit(-1);
	}


	HWND fenetre = FindWindowA(NULL, "Cisco Webex Meetings"); //Récupération de la fenêtre de Cisco
	
	//SetForegroundWindow(fenetre); //Si on veut mettre Cisco devant la console
	
	if (fenetre == NULL) {
		cout << "Impossible de trouver la fenetre du logiciel!!!\n";
		MessageBox(NULL, (LPCUWSTR)L"Impossible de trouver la fenêtre du logiciel!", (LPCUWSTR)L"Erreur", MB_ICONERROR | MB_OK);
		exit(-1);
	}
	else {
		SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY);
		cout << "Recuperation de la fenetre de Cisco..." << endl;
		DWORD IDduProc;
		GetWindowThreadProcessId(fenetre, &IDduProc); //Récupération de l'ID du processus de Cisco
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, IDduProc); //Création d'un handle sur cette fenêtre
		if (IDduProc == NULL) {
			cout << "Impossible de trouver l'indentifiant du processus!!!\n";
			MessageBox(NULL, (LPCUWSTR)L"Impossible de trouver l'indentifiant du processus de Cisco, vérifiez que le logiciel est bien lancé et que vous êtes bien en réunion!", (LPCUWSTR)L"Erreur", MB_ICONERROR | MB_OK);
			Sleep(3000);
			exit(-1);
		}
		else {
			cout << "Recuperation de l'indentifiant du processus de Cisco..." << endl;
			
			int adressedll = GetModuleBaseAddress(IDduProc, L"mac.dll"); //Récupération de l'adresse du DLL de Cisco ici
			cout << "Recuperation du DLL gestion vocale de Cisco..." << endl;
			//Débug
			//cout << "Adresse du dll: " << adressedll << "\n";

	
			//Creation des tableaux de stockage des offsets
			int pointeurs[200];
			string versions_list[20];
			//Création des itérateurs pour après
			int iterateur = 0;
			int sous_iterateur = 0;


			//Récupération de la liste des versions
			cout << "Telechargement de la liste des versions supportees" << endl;
			URLDownloadToFile(NULL, lien_temp_file_url, lien_temp_file, 0, NULL);

			//Lecture de la liste des versions
			fstream file;
			string word, t, q, delim;
			delim = "-";


			//Ouverture fichier
			file.open(temppath_string.c_str());

			//Extraction de chacun des mots du fichier
			while (file >> word)
			{
				//Separation des différentes parties du string
				auto start = 0U;
				auto end = word.find(delim);
				while (end != std::string::npos) //Tant qu'on est pas arrivé au bout du fichier
				{
					string a_stock = word.substr(start, end - start);
					if (sous_iterateur != 0) {
						pointeurs[iterateur * 10 + sous_iterateur] = stoi(a_stock); //Stockage des pointeurs de cette version
					}
					else {
						versions_list[iterateur] = a_stock; //Stockage de la version
					}
					sous_iterateur++;
					//Débug
					//cout << word.substr(start, end - start) << std::endl;
					start = end + delim.length();
					end = word.find(delim, start);
				}
				//Débug
				//std::cout << word.substr(start, end);
				iterateur++;
				sous_iterateur = 0;
				
			}
			
			//Débug
			//cout << "versions_list[1] " << versions_list[0] << endl;
			//cout << "pointeurs[1] " << pointeurs[1] << endl;
			//cout << "versions_list[2] " << versions_list[1] << endl;
			//cout << "pointeurs[11] " << pointeurs[11] << endl;
			
			//Initialisation des offsets
			int offset1;
			int offset2;
			int offset3;
			int offset4;
			int offset5;
			int offset6;

			SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN); //En VERT
			cout << "Entrez la version que vous voulez utiliser:" << endl;
			SetConsoleTextAttribute(hStdOut, 15); //En Blanc
			cout << "En vert les versions potentiellement compatibles et en rouge les versions non compatibles. Verifiez votre version avant de choisir!!!" << endl;
			int compteur = 0;
			int test_output;
				
			while (versions_list[compteur] != "") { //Itération de l'ensemble des versions trouvées
				
				int test_memorytoread;
				//Assignation des pointeurs
				offset1 = pointeurs[compteur * 10 + 1];
				offset2 = pointeurs[compteur * 10 + 2];
				offset3 = pointeurs[compteur * 10 + 3];
				offset4 = pointeurs[compteur * 10 + 4];
				offset5 = pointeurs[compteur * 10 + 5];
				offset6 = pointeurs[compteur * 10 + 6];
				
				//Lecture RAM des pointeurs (pour obtenir le pointeur suivant et enfin trouver la valeur
				ReadProcessMemory(handle, (LPCVOID)(adressedll + offset1), &test_memorytoread, sizeof(test_memorytoread), NULL);
				//cout << "memorytoread: " << std::hex << test_memorytoread << endl;
				ReadProcessMemory(handle, (LPCVOID)(test_memorytoread + offset2), &test_memorytoread, sizeof(test_memorytoread), NULL);
				//cout << "memorytoread: " << std::hex << test_memorytoread << endl;
				ReadProcessMemory(handle, (LPCVOID)(test_memorytoread + offset3), &test_memorytoread, sizeof(test_memorytoread), NULL);
				//cout << "memorytoread: " << std::hex << test_memorytoread << endl;
				ReadProcessMemory(handle, (LPCVOID)(test_memorytoread + offset4), &test_memorytoread, sizeof(test_memorytoread), NULL);
				//cout << "memorytoread: " << std::hex << test_memorytoread << endl;
				ReadProcessMemory(handle, (LPCVOID)(test_memorytoread + offset5), &test_memorytoread, sizeof(test_memorytoread), NULL);
				//cout << "memorytoread: " << std::hex << test_memorytoread << endl;
				
				int test_adressemic = test_memorytoread + offset6; //Ajout du dernier offset sur le pointeur trouvé dans la dernière case mémoire
				
				int test_ValueMicro = 0;
				
				bool valide;
				
				if (test_adressemic < offset6 + offset5) { //Vérification de routine de la justesse de la lecture
					valide = false;
				}
				else {
					valide = true;
				}
				if (valide) { //Si vérification effectuée, alors on vérifie la valeur dans la case finale
					ReadProcessMemory(handle, (LPCVOID)test_adressemic, &test_ValueMicro, sizeof(test_ValueMicro), NULL);
				}
				//Débug
				//cout << "Val micro adr: " << test_adressemic << " test "<< compteur << ": " << test_ValueMicro << endl;
				
				if ((test_ValueMicro == 0 || test_ValueMicro == 1) && valide) { //Si on trouve une valeur qui pourrait potentiellement coller, on met la version en vert
					SetConsoleTextAttribute(hStdOut, 10);
				}
				else { //Sinon, on la met en rouge
					SetConsoleTextAttribute(hStdOut, 12);
				}

				cout << compteur << ": " << versions_list[compteur] << endl; //On affiche ensuite la version
				compteur++;
			}
			
			SetConsoleTextAttribute(hStdOut, 15); //En blanc
			cout << "? -> "; //Demande de la version choisie
			int version_choisie;
			cin >> version_choisie;
			if (version_choisie >= 0 && version_choisie < compteur) { //Si la version choisie existe
				
				cout << "Vous avez choisi la version: " << versions_list[version_choisie] << endl;
				//On met les bon offsets
				offset1 = pointeurs[version_choisie * 10 + 1];
				offset2 = pointeurs[version_choisie * 10 + 2];
				offset3 = pointeurs[version_choisie * 10 + 3];
				offset4 = pointeurs[version_choisie * 10 + 4];
				offset5 = pointeurs[version_choisie * 10 + 5];
				offset6 = pointeurs[version_choisie * 10 + 6];

			}
			else { //Sinon on ferme
				cout << "Erreur dans la selection de la version!" << endl;
				exit(-1);
			}
			
			//Anciens offsets de test:
			
			//Cisco 40.8.7.13
			//int offset1 = 0x001854A8;
			/*
			int offset2 = 0x20;
			int offset3 = 0x5C;
			int offset4 = 0x1C;
			int offset5 = 0x1F8;
			int offset6 = 0xF4;
			*/

			//Cisco 40.9.3.20
			/*
			int offset1 = 0x001884A8;
			int offset2 = 0x20;
			int offset3 = 0x44;
			int offset4 = 0xF8;
			int offset5 = 0x1F8;
			int offset6 = 0xF4;
			*/
			
			//On va chercher le pointeur vers la valeur du micro
			int memorytoread;

			ReadProcessMemory(handle, (LPCVOID)(adressedll + offset1), &memorytoread, sizeof(memorytoread), NULL);
			//cout << "memorytoread: " << std::hex << memorytoread << endl;
			ReadProcessMemory(handle, (LPCVOID)(memorytoread + offset2), &memorytoread, sizeof(memorytoread), NULL);
			//cout << "memorytoread: " << std::hex << memorytoread << endl;
			ReadProcessMemory(handle, (LPCVOID)(memorytoread + offset3), &memorytoread, sizeof(memorytoread), NULL);
			//cout << "memorytoread: " << std::hex << memorytoread << endl;
			ReadProcessMemory(handle, (LPCVOID)(memorytoread + offset4), &memorytoread, sizeof(memorytoread), NULL);
			//cout << "memorytoread: " << std::hex << memorytoread << endl;
			ReadProcessMemory(handle, (LPCVOID)(memorytoread + offset5), &memorytoread, sizeof(memorytoread), NULL);
			//cout << "memorytoread: " << std::hex << memorytoread << endl;
			int adressemic = memorytoread + offset6;
			//Débug
			//cout << "memorytoread: " << std::hex << adressemic << endl;

			HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY); //En gris
			cout << endl;
			//Affichage d'un débug pour aider à distance
			cout << "Debug d'aide (Pour support aupres de Pierre):" << endl << "Adresse du DLL mac.dll (Semi-Static Pointer): 0x" << std::hex << adressedll << endl;
			cout << "Offsets: 0x" << std::hex << offset1 << " 0x" << offset2 << " 0x" << offset3 << " 0x" << offset4 << " 0x" << offset5 << " 0x" << offset6 << " fin des offsets." << endl;
			cout << "Pointeur STATIC final de l'injection: 0x" << std::hex << adressemic << endl << endl;

			
			int valueMicro;
			SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY);
			ReadProcessMemory(handle, (LPCVOID)adressemic, &valueMicro, sizeof(valueMicro), NULL); //On lit la valeur du micro
			//1 = Micro muté
			//0 = Micro actif
			
			//Débug
			//cout << "Val micro: " << valueMicro << endl;
			
			if (!(valueMicro == 0 || valueMicro == 1)) { //Si la valeur lue le colle pas avec les attentes
				MessageBox(NULL, (LPCUWSTR)L"Vous ne devez laisser ouvert sur Cisco que la fenêtre de la réunion lors de l'initialisation de ce logiciel!\nIl se peut également que Cisco ait été mis à jour, assurez vous d'être dans la bonne version!", (LPCUWSTR)L"Erreur", MB_ICONERROR | MB_OK);
				exit(-1);
			}
			else {
				//Sinon, OK!
				SetConsoleTextAttribute(hStdOut, 15);
				cout << "Initialisation achevee! Vous pouvez continuer ce que vous faisiez!" << endl;
				cout << "Si vous souhaitez pouvoir parler, appuyez sur F2!" << endl;
				SetConsoleTextAttribute(hStdOut, FOREGROUND_RED);
				cout << "Blocage ON" << endl;

			}
			bool speak = false;
			for (;;) { //Entrée dans la boucle infinie du programme
				HWND fenetre_tjr_la = FindWindowA(NULL, "Cisco Webex Meetings"); //On verifie a chaque fois que le programme est toujours bien lancé pour éviter de modifier la mémoire d'un autre programme
				if (fenetre_tjr_la == NULL) {
					cout << "Fermeture de Cisco, arret du logiciel\n";
					exit(-1);
				}
				if (GetAsyncKeyState(VK_F2) != 0) //Si l'utilisateur appuie sur F2
				{
					if (speak) { //Si le blocage était sur OFF
						speak = false;
						SetConsoleTextAttribute(hStdOut, FOREGROUND_RED);
						cout << "Blocage ON" << endl;
					}
					else { //Sinon on le met sur OFF
						speak = true;
						SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN);
						cout << "Blocage OFF" << endl;
					}
					
					Sleep(200); //Pour éviter qu'un simple appuie déclenche plusieurs fois le IF
				}
				if (!speak) { //Si l'utilisateur est en blocage actif
					int valeur = 1;
					WriteProcessMemory(handle, (LPVOID)adressemic, &valeur, sizeof(valeur), 0); //On force Cisco à bloquer la voix
				}
				else {
					int valeur = 0;
					WriteProcessMemory(handle, (LPVOID)adressemic, &valeur, sizeof(valeur), 0); //On force Cisco à débloquer la voix (si l'utilisateur est démute)!
				}
				
			}
			
		}
		
	}
	return 0; //Toujour un return 0! (même si on devrait jamais l'atteindre!)
}
