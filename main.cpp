#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <map>
#include <vector>
#include <locale>
#include <unordered_map>
#include <cctype>

std::vector<std::wstring> SurastiURLs(const std::wstring& tekstas) {
    std::wregex urlRegex(L"(https?://\\S+|www\\.\\S+)");
    std::wsmatch atitikmuo;
    std::vector<std::wstring> urls;

    std::wstring::const_iterator pradzia = tekstas.begin();
    std::wstring::const_iterator pabaiga = tekstas.end();

    while (std::regex_search(pradzia, pabaiga, atitikmuo, urlRegex)) {
        urls.push_back(atitikmuo[0]);
        pradzia = atitikmuo.suffix().first;
    }

    return urls;
}

std::wstring PanaikintiPaskutiniSkyrybosZenklus(const std::wstring& zodis) {
    size_t paskutinisSimbolis = zodis.size() - 1;

    while (paskutinisSimbolis > 0 && !iswalpha(zodis[paskutinisSimbolis])) {
        --paskutinisSimbolis;
    }

    return zodis.substr(0, paskutinisSimbolis + 1);
}

std::wstring ApdorotiZodi(const std::wstring& zodis) {
    std::wstring apdorotasZodis = PanaikintiPaskutiniSkyrybosZenklus(zodis);
    std::locale ltLoc("lt_LT.UTF-8");

    apdorotasZodis = std::regex_replace(apdorotasZodis, std::wregex(L"[^a-zA-Ząčęėįšųūž]"), L"");

    for (wchar_t& simbolis : apdorotasZodis) {
        simbolis = std::tolower(simbolis, ltLoc);
    }

    return apdorotasZodis;
}

std::map<std::wstring, int> SkaiciuotiZodzioPasikartojimus(const std::wstring& tekstas) {
    std::map<std::wstring, int> zodzioPasikartojimai;
    std::wistringstream srautas(tekstas);
    std::wstring zodis;

    while (srautas >> zodis) {
        zodis = ApdorotiZodi(zodis);

        if (!zodis.empty()) {
            zodzioPasikartojimai[zodis]++;
        }
    }

    return zodzioPasikartojimai;
}

std::unordered_map<std::wstring, std::vector<int>> SukurtiKryzmineNuoroduLentele(const std::wstring& tekstas) {
    std::unordered_map<std::wstring, std::vector<int>> kryzmineNuoroduLentele;
    std::wregex zodzioRegex(L"[a-zA-Ząčęėįšųūž]+");
    std::wsmatch atitikmuo;

    std::wistringstream srautas(tekstas);
    std::wstring line;
    int eilutesNumeris = 1;

    while (std::getline(srautas, line)) {
        std::wstring::const_iterator pradzia = line.begin();
        std::wstring::const_iterator pabaiga = line.end();

        while (std::regex_search(pradzia, pabaiga, atitikmuo, zodzioRegex)) {
            std::wstring zodis = atitikmuo[0];
            std::transform(zodis.begin(), zodis.end(), zodis.begin(), towlower);

            if (kryzmineNuoroduLentele[zodis].empty() || kryzmineNuoroduLentele[zodis].back() != eilutesNumeris) {
                kryzmineNuoroduLentele[zodis].push_back(eilutesNumeris);
            }

            pradzia = atitikmuo.suffix().first;
        }

        ++eilutesNumeris;
    }

    for (auto it = kryzmineNuoroduLentele.begin(); it != kryzmineNuoroduLentele.end();) {
        if (it->second.size() < 2) {
            it = kryzmineNuoroduLentele.erase(it);
        }
        else {
            ++it;
        }
    }

    std::map<std::wstring, std::vector<int>> surusiuotiZodziai(kryzmineNuoroduLentele.begin(), kryzmineNuoroduLentele.end());

    kryzmineNuoroduLentele.clear();

    for (const auto& pora : surusiuotiZodziai) {
        kryzmineNuoroduLentele.insert(pora);
    }

    return kryzmineNuoroduLentele;
}
//papildoma dalis
std::vector<std::wstring> RastiZodziusSuDalim(const std::wstring& tekstas, const std::wstring& substring) {
    std::wregex zodzioSuDalimRegex(L"\\w*" + substring + L"\\w*");
    std::wsmatch atitikmuo;
    std::vector<std::wstring> zodziaiSuDalim;

    std::wstring::const_iterator pradzia = tekstas.begin();
    std::wstring::const_iterator pabaiga = tekstas.end();

    while (std::regex_search(pradzia, pabaiga, atitikmuo, zodzioSuDalimRegex)) {
        zodziaiSuDalim.push_back(atitikmuo[0]);
        pradzia = atitikmuo.suffix().first;
    }

    return zodziaiSuDalim;
}




int main() {
    std::locale::global(std::locale("lt_LT.UTF-8"));
    std::wcout.imbue(std::locale());

    std::wcout << L"Iveskite failo varda: ";
    std::wstring failoVardas;
    std::wcin >> failoVardas;

    std::wifstream failoSrautas(failoVardas);

    if (!failoSrautas.is_open()) {
        std::wcerr << L"Nepavyko atidaryti failo!" << std::endl;
        return EXIT_FAILURE;
    }

    std::wstringstream tekstoSrautas;
    tekstoSrautas << failoSrautas.rdbuf();
    std::wstring tekstas = tekstoSrautas.str();


    std::vector<std::wstring> urls = SurastiURLs(tekstas);

    std::wcout << L"Rasti URL'ai:" << std::endl;
    for (const std::wstring& url : urls) {
        std::wcout << url << std::endl;
    }

    for (const std::wstring& url : urls) {
        size_t rasta = tekstas.find(url);
        while (rasta != std::wstring::npos) {
            tekstas.replace(rasta, url.length(), L"");
            rasta = tekstas.find(url);
        }
    }

    std::map<std::wstring, int> zodzioPasikartojimai = SkaiciuotiZodzioPasikartojimus(tekstas);

    std::wcout << L"\nŽodžių pasikartojimai:" << std::endl;
    for (const auto& pora : zodzioPasikartojimai) {
        if (pora.second > 1) {
            std::wcout << L"Žodis: " << pora.first << L", Pasikartoja: " << pora.second << L" kartus" << std::endl;
        }
    }

    std::unordered_map<std::wstring, std::vector<int>> kryzmineNuoroduLentele = SukurtiKryzmineNuoroduLentele(tekstas);

    std::wcout << L"\nKryžminė nuorodų lentelė:" << std::endl;
    for (const auto& pora : kryzmineNuoroduLentele) {
        std::wcout << L"Žodis: " << pora.first << L", Eilutės: ";
        for (int eilutesNumeris : pora.second) {
            std::wcout << eilutesNumeris << L" ";
        }
        std::wcout << std::endl;
    }
    //papildoma dalis
    std::vector<std::wstring> zodziaiSuDalim = RastiZodziusSuDalim(tekstas, L"avo");

    std::wcout << L"\nŽodžiai su dalim \"avo\":" << std::endl;
    for (const std::wstring& zodis : zodziaiSuDalim) {
        std::wcout << zodis << std::endl;
    }


    return EXIT_SUCCESS;
}
