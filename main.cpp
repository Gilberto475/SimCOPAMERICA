#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <algorithm>

using namespace std;

struct Partido {
    string equipo1;
    string equipo2;
    int golesEquipo1;
    int golesEquipo2;
    string estadio;
};

void simularResultados(vector<Partido>& partidos) {
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> distrib(0, 5);

    for (auto& partido : partidos) {
        partido.golesEquipo1 = distrib(rng);
        partido.golesEquipo2 = distrib(rng);
    }
}

int obtenerPuntos(int goles1, int goles2) {
    if (goles1 > goles2) {
        return 3;  // Victoria
    } else if (goles1 < goles2) {
        return 0;  // Derrota
    } else {
        return 1;  // Empate
    }
}

int main() {
    ifstream archivo("equipos.txt");
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo equipos.txt" << endl;
        return 1;
    }

    unordered_map<string, vector<vector<Partido>>> grupos;
    vector<vector<Partido>> jornadas;
    vector<Partido> jornada_actual;
    string linea;
    string grupo_actual = "";

    while (getline(archivo, linea)) {
        linea.erase(0, linea.find_first_not_of(" \t\r\n"));
        linea.erase(linea.find_last_not_of(" \t\r\n") + 1);

        if (linea.empty()) {
            continue;
        }

        if (linea[0] == '#') {  // Indica el inicio de un nuevo grupo
            if (!jornadas.empty()) {
                grupos[grupo_actual] = jornadas;
                jornadas.clear();
            }
            grupo_actual = linea.substr(1);  // Obtener el nombre del grupo
        } else if (linea == ";") {
            if (!jornada_actual.empty()) {
                jornadas.push_back(jornada_actual);
                jornada_actual.clear();
            }
        } else {
            size_t colon_pos = linea.find(":");
            if (colon_pos != string::npos) {
                string partidos_info = linea.substr(0, colon_pos);
                string estadio = linea.substr(colon_pos + 1);

                size_t vs_pos = partidos_info.find(" vs ");
                if (vs_pos != string::npos) {
                    string equipo1 = partidos_info.substr(0, vs_pos);
                    string equipo2 = partidos_info.substr(vs_pos + 4);

                    Partido partido{equipo1, equipo2, 0, 0, estadio};
                    jornada_actual.push_back(partido);
                }
            }
        }
    }

    if (!jornada_actual.empty()) {
        jornadas.push_back(jornada_actual);
    }

    if (!grupo_actual.empty() && !jornadas.empty()) {
        grupos[grupo_actual] = jornadas;
    }

    // Procesar cada grupo
    for (auto& [grupo, jornadas] : grupos) {
        // Simular los resultados de cada jornada
        for (auto& jornada : jornadas) {
            simularResultados(jornada);
        }

        // Mostrar los resultados simulados para cada jornada
        cout << "Resultados del Grupo " << grupo << ":\n";
        for (size_t i = 0; i < jornadas.size(); ++i) {
            cout << "Jornada " << i + 1 << ":\n";
            for (const auto& partido : jornadas[i]) {
                cout << partido.equipo1 << " " << partido.golesEquipo1 << " - "
                     << partido.golesEquipo2 << " " << partido.equipo2
                     << " (Estadio: " << partido.estadio << ")\n";
            }
            cout << endl; // Espacio entre jornadas
        }

        // Calcular puntos por cada equipo en el grupo
        unordered_map<string, int> puntosPorEquipo;

        for (const auto& jornada : jornadas) {
            for (const auto& partido : jornada) {
                int puntosEquipo1 = obtenerPuntos(partido.golesEquipo1, partido.golesEquipo2);
                int puntosEquipo2 = obtenerPuntos(partido.golesEquipo2, partido.golesEquipo1);

                puntosPorEquipo[partido.equipo1] += puntosEquipo1;
                puntosPorEquipo[partido.equipo2] += puntosEquipo2;
            }
        }

        // Convertir el mapa en un vector para ordenar por puntos
        vector<pair<string, int>> equipoPuntos(puntosPorEquipo.begin(), puntosPorEquipo.end());

        // Ordenar de mayor a menor por puntos
        sort(equipoPuntos.begin(), equipoPuntos.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
        });

        // Mostrar los dos primeros equipos que pasan a la siguiente ronda
        cout << "Equipos que pasan a la siguiente ronda del Grupo " << grupo << ":\n";
        for (int i = 0; i < 2 && i < equipoPuntos.size(); ++i) {
            cout << equipoPuntos[i].first << " con " << equipoPuntos[i].second << " puntos" << endl;
        }

        cout << "\n";  // Espacio entre grupos
    }

    return 0;
}
