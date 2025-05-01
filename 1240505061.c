
/**
 * SOSYAL AĞ ANALİZ SİSTEMİ
 *
 * Bu program, kullanıcılar arasındaki ilişkileri Red-Black Tree ve Graf yapılarıyla modelleyerek:
 * - Arkadaşlık ilişkilerini yönetir
 * - Topluluk tespiti yapar
 * - Etki alanı analizi gerçekleştirir
 *
 * Veri Yapıları:
 * 1. Red-Black Tree: Kullanıcıların kimlik numaralarına göre düzenli bir şekilde saklanması için kullanılıyor.
 * Bu yapı sayesinde kullanıcılar kolay bir şekilde hızlıca eklenebiliyor kimliğe gore hızlıca arama yapılabiliyor.
 * dengeli bir ikili arama ağacı olduğu için bu yapı kullanılıyor
 * 2. Graf: Kullanıcılar arası ilişkilerin temsilinde kullanılıyor.
 * Graf yapısı ile birlikte DFS ile topluluk tespiti yapılabilir ve
 * ortak arkadaş bulma, mesafeye göre arkadaş önerme gibi analizler yapılabilir.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARKADAS 1000 // Bir kullanıcının maksimum arkadaş sayısı

#define MAX_KULLANICI 1000 // Sistemdeki maksimum kullanıcı sayısı

// Red-Black Tree düğüm renkleri
typedef enum { KIRMIZI, SIYAH } Renk;

// Kullanıcı yapısı
typedef struct Kullanici {
    int kimlik;// Kullanıcı ID'si
    struct Kullanici* arkadaslar[MAX_ARKADAS];
    int arkadasSayisi;
    int toplulukID; // Topluluk belirleme için
} Kullanici;

// Red-Black Tree düğüm yapısı
typedef struct RBNode {
    Kullanici* kullanici;// Kullanıcı verisi
    Renk renk; // Düğüm rengi
    struct RBNode *sol, *sag, *ebeveyn; // Ağaç yapısı
} RBNode;

RBNode* kok = NULL;

//--------------------------------------
// Topluluk Tespiti için Yardımcı Yapılar
//--------------------------------------
int ziyaretEdilen[MAX_KULLANICI] = {0}; // DFS'de ziyaret kontrolü
int toplulukSayaci = 0;              // Bulunan topluluk sayısı


//--------------------------------------
// RB Tree Yardımcı Fonksiyonlar
//--------------------------------------

/*
 * Yeni RB düğümü oluşturur
 * k: Eklenecek kullanıcı pointerı
 * return: Oluşturulan düğüm
 */
RBNode* yeniRBNode(Kullanici* k) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    node->kullanici = k;
    node->renk = KIRMIZI;// Yeni düğümler her zaman kırmızı
    node->sol = node->sag = node->ebeveyn = NULL;
    return node;
}
/*
 * Sola dönüş işlemi (RB Tree dengeleme)
 *  kok: Ağaç kökü
 *  x: Döndürülecek düğüm
 */
void solRotate(RBNode** kok, RBNode* x) {
    RBNode* y = x->sag;
    x->sag = y->sol;
    if (y->sol != NULL)
        y->sol->ebeveyn = x;
    y->ebeveyn = x->ebeveyn;
    if (x->ebeveyn == NULL)
        *kok = y;
    else if (x == x->ebeveyn->sol)
        x->ebeveyn->sol = y;
    else
        x->ebeveyn->sag = y;
    y->sol = x;
    x->ebeveyn = y;
}
/*
 * Sağa dönüş işlemi (RB Tree dengeleme)
 *  kok: Ağaç kökü
 *  y: Döndürülecek düğüm
 */
void sagRotate(RBNode** kok, RBNode* y) {
    RBNode* x = y->sol;
    y->sol = x->sag;
    if (x->sag != NULL)
        x->sag->ebeveyn = y;
    x->ebeveyn = y->ebeveyn;
    if (y->ebeveyn == NULL)
        *kok = x;
    else if (y == y->ebeveyn->sol)
        y->ebeveyn->sol = x;
    else
        y->ebeveyn->sag = x;
    x->sag = y;
    y->ebeveyn = x;
}
/*
 * RB Tree renk düzenleme (Insertion sonrası dengeleme)
 *  kok: Ağaç kökü
 *  z: Yeni eklenen düğüm
 */
void renkDuzenle(RBNode** kok, RBNode* z) {
    while (z != *kok && z->ebeveyn->renk == KIRMIZI) {
        if (z->ebeveyn == z->ebeveyn->ebeveyn->sol) {
            RBNode* y = z->ebeveyn->ebeveyn->sag;
            if (y != NULL && y->renk == KIRMIZI) {
                z->ebeveyn->renk = SIYAH;
                y->renk = SIYAH;
                z->ebeveyn->ebeveyn->renk = KIRMIZI;
                z = z->ebeveyn->ebeveyn;
            } else {
                if (z == z->ebeveyn->sag) {
                    z = z->ebeveyn;
                    solRotate(kok, z);
                }
                z->ebeveyn->renk = SIYAH;
                z->ebeveyn->ebeveyn->renk = KIRMIZI;
                sagRotate(kok, z->ebeveyn->ebeveyn);
            }
        } else {
            RBNode* y = z->ebeveyn->ebeveyn->sol;
            if (y != NULL && y->renk == KIRMIZI) {
                z->ebeveyn->renk = SIYAH;
                y->renk = SIYAH;
                z->ebeveyn->ebeveyn->renk = KIRMIZI;
                z = z->ebeveyn->ebeveyn;
            } else {
                if (z == z->ebeveyn->sol) {
                    z = z->ebeveyn;
                    sagRotate(kok, z);
                }
                z->ebeveyn->renk = SIYAH;
                z->ebeveyn->ebeveyn->renk = KIRMIZI;
                solRotate(kok, z->ebeveyn->ebeveyn);
            }
        }
    }
    (*kok)->renk = SIYAH;
}
/*
 * RB Tree'ye yeni kullanıcı ekler
 *  kok: Ağaç kökü
 *  k: Eklenecek kullanıcı
 */
void rbEkle(RBNode** kok, Kullanici* k) {
    RBNode* z = yeniRBNode(k);
    RBNode* y = NULL;
    RBNode* x = *kok;

    while (x != NULL) {
        y = x;
        if (z->kullanici->kimlik < x->kullanici->kimlik)
            x = x->sol;
        else
            x = x->sag;
    }

    z->ebeveyn = y;
    if (y == NULL)
        *kok = z;
    else if (z->kullanici->kimlik < y->kullanici->kimlik)
        y->sol = z;
    else
        y->sag = z;

    renkDuzenle(kok, z);
}
/*
 * RB Tree'de kullanıcı arar
 *  kok: Arama başlangıç düğümü
 *  kimlik: Aranacak kullanıcı ID'si
 * return: Bulunan düğüm veya NULL
 */
RBNode* rbAra(RBNode* kok, int kimlik) {
    if (kok == NULL || kok->kullanici->kimlik == kimlik)
        return kok;

    if (kimlik < kok->kullanici->kimlik)
        return rbAra(kok->sol, kimlik);
    else
        return rbAra(kok->sag, kimlik);
}

//--------------------------------------
// Kullanıcı İşlemleri
//--------------------------------------
/*
 * Yeni kullanıcı oluşturur
 *  kimlik: Kullanıcı ID'si
 * return: Oluşturulan kullanıcı
 */
Kullanici* kullaniciOlustur(int kimlik) {
    Kullanici* yeni = (Kullanici*)malloc(sizeof(Kullanici));
    yeni->kimlik = kimlik;
    yeni->arkadasSayisi = 0;
    return yeni;
}
/*
 * Kullanıcı bulur veya yoksa oluşturur
 *  kimlik: Aranacak kullanıcı ID'si
 * return: Bulunan/oluşturulan kullanıcı
 */
Kullanici* kullaniciBulVeyaOlustur(int kimlik) {
    RBNode* node = rbAra(kok, kimlik);
    if (node != NULL) {
        return node->kullanici;
    } else {
        Kullanici* yeni = kullaniciOlustur(kimlik);
        rbEkle(&kok, yeni);
        return yeni;
    }
}
/*
 * İki kullanıcı arasında arkadaşlık ilişkisi kurar (graf kenarı ekler)
 * a: Birinci kullanıcı
 *  b: İkinci kullanıcı
 */
void arkadasEkle(Kullanici* a, Kullanici* b) {
    if (a->arkadasSayisi < MAX_ARKADAS && b->arkadasSayisi < MAX_ARKADAS) {
        a->arkadaslar[a->arkadasSayisi++] = b;
        b->arkadaslar[b->arkadasSayisi++] = a;
    }
}

//--------------------------------------
// Dosya İşlemleri
//--------------------------------------
/*
 * Dosyadan kullanıcı ve arkadaşlık verilerini okur
 */
void dosyaOku(const char* dosyaAdi) {
    FILE* dosya = fopen(dosyaAdi, "r");
    if (!dosya) {
        perror("Dosya acilamadi");
        exit(1);
    }
    char tur[10];
    int kimlik1, kimlik2;
    while (fscanf(dosya, "%s", tur) != EOF) {
        if (strcmp(tur, "USER") == 0) {
            fscanf(dosya, "%d", &kimlik1);
            kullaniciBulVeyaOlustur(kimlik1);
        } else if (strcmp(tur, "FRIEND") == 0) {
            fscanf(dosya, "%d %d", &kimlik1, &kimlik2);
            Kullanici* k1 = kullaniciBulVeyaOlustur(kimlik1);
            Kullanici* k2 = kullaniciBulVeyaOlustur(kimlik2);
            arkadasEkle(k1, k2);
        }
    }
    fclose(dosya);
}

//--------------------------------------
// DFS ile Arkadaş Bulma (RB Tree uyumlu)
//--------------------------------------
void dfs(RBNode* node, int* ziyaretEdilen, int currentDepth, int targetDepth) {
    if (node == NULL || currentDepth > targetDepth) return;

    if (currentDepth == targetDepth) {
        printf("%d\n", node->kullanici->kimlik);
        return;
    }

    ziyaretEdilen[node->kullanici->kimlik] = 1;

    for (int i = 0; i < node->kullanici->arkadasSayisi; i++) {
        int arkadasKimlik = node->kullanici->arkadaslar[i]->kimlik;
        if (!ziyaretEdilen[arkadasKimlik]) {
            RBNode* arkadasNode = rbAra(kok, arkadasKimlik);
            if (arkadasNode != NULL) {
                dfs(arkadasNode, ziyaretEdilen, currentDepth + 1, targetDepth);
            }
        }
    }
}
/*
 * Belirli mesafedeki arkadaşları bulur (BFS benzeri)
 *  kimlik: Başlangıç kullanıcı ID'si
 *  mesafe: Aranacak arkadaşlık mesafesi
 */
void belirliMesafedekiArkadaslariBul(int kimlik, int mesafe) {
    if (mesafe <= 0) {
        printf("Mesafe pozitif bir tam sayi olmalidir.\n");
        return;
    }

    RBNode* baslangicNode = rbAra(kok, kimlik);
    if (baslangicNode == NULL) {
        printf("Kullanici bulunamadi.\n");
        return;
    }

    int ziyaretEdilen[MAX_ARKADAS * 2] = {0}; // Büyük bir değer kullanıldı
    printf("%d kullanicisinin %d mesafedeki arkadaslari:\n", kimlik, mesafe);
    dfs(baslangicNode, ziyaretEdilen, 0, mesafe);
}

//--------------------------------------
// Ortak Arkadaş Bulma (RB Tree uyumlu)
//--------------------------------------
/*
 * İki kullanıcının ortak arkadaşlarını bulur
 *  kimlik1: Birinci kullanıcı
 *  kimlik2: İkinci kullanıcı
 */
void ortakArkadaslariBul(int kimlik1, int kimlik2) {
    RBNode* node1 = rbAra(kok, kimlik1);
    RBNode* node2 = rbAra(kok, kimlik2);

    if (node1 == NULL || node2 == NULL) {
        printf("Kullanici bulunamadi.\n");
        return;
    }

    printf("Ortak arkadaslar: ");
    int yazdirilan[MAX_ARKADAS * 2] = {0};

    for (int i = 0; i < node1->kullanici->arkadasSayisi; i++) {
        for (int j = 0; j < node2->kullanici->arkadasSayisi; j++) {
            if (node1->kullanici->arkadaslar[i]->kimlik == node2->kullanici->arkadaslar[j]->kimlik &&
                !yazdirilan[node1->kullanici->arkadaslar[i]->kimlik]) {
                printf("%d ", node1->kullanici->arkadaslar[i]->kimlik);
                yazdirilan[node1->kullanici->arkadaslar[i]->kimlik] = 1;
            }
        }
    }
    printf("\n");
}

//--------------------------------------
// Etki Alanı Hesaplama (RB Tree uyumlu)
//--------------------------------------
/*
 * Bir kullanıcının etki alanındaki kişi sayısını hesaplar
 *  kimlik: Analiz edilecek kullanıcı
 */
int erisebilirlikDFS(RBNode* node, int* ziyaretEdilen) {
    if (node == NULL) return 0;

    int sayac = 1;
    ziyaretEdilen[node->kullanici->kimlik] = 1;

    for (int i = 0; i < node->kullanici->arkadasSayisi; i++) {
        int arkadasKimlik = node->kullanici->arkadaslar[i]->kimlik;
        if (!ziyaretEdilen[arkadasKimlik]) {
            RBNode* arkadasNode = rbAra(kok, arkadasKimlik);
            if (arkadasNode != NULL) {
                sayac += erisebilirlikDFS(arkadasNode, ziyaretEdilen);
            }
        }
    }
    return sayac;
}

void etkiAlaniniHesapla(int kimlik) {
    RBNode* node = rbAra(kok, kimlik);
    if (node == NULL) {
        printf("Kullanici bulunamadi.\n");
        return;
    }

    int ziyaretEdilen[MAX_ARKADAS * 2] = {0};
    int erisebilen = erisebilirlikDFS(node, ziyaretEdilen) - 1;
    printf("Kullanici %d, %d kisiye erisebilir.\n", kimlik, erisebilen);
}

//--------------------------------------
// Bellek Temizleme
//--------------------------------------
void agaciTemizle(RBNode* node) {
    if (node == NULL) return;
    agaciTemizle(node->sol);
    agaciTemizle(node->sag);
    free(node->kullanici);
    free(node);
}

//--------------------------------------
// Topluluk Tespit Fonksiyonları
//--------------------------------------

// Derinlik Öncelikli Arama ile toplulukları belirle
/*
 * DFS ile bağlı bileşenleri (toplulukları) işaretler
 * node: Başlangıç düğümü
 *  toplulukID: Atanacak topluluk ID'si
 */
void dfsToplulukTespit(RBNode* node, int toplulukID) {
    if (node == NULL || ziyaretEdilen[node->kullanici->kimlik]) return;

    ziyaretEdilen[node->kullanici->kimlik] = 1;
    node->kullanici->toplulukID = toplulukID;

    for (int i = 0; i < node->kullanici->arkadasSayisi; i++) {
        int arkadasKimlik = node->kullanici->arkadaslar[i]->kimlik;
        RBNode* arkadasNode = rbAra(kok, arkadasKimlik);
        if (arkadasNode != NULL) {
            dfsToplulukTespit(arkadasNode, toplulukID);
        }
    }
}

// Tüm kullanıcılar için toplulukları belirle
void topluluklariBelirle() {
    // Ziyaret edilen dizisini sıfırla
    memset(ziyaretEdilen, 0, sizeof(ziyaretEdilen));
    toplulukSayaci = 0;

    // RB Tree'yi inorder gezerek toplulukları belirle
    RBNode* stack[MAX_KULLANICI];
    int stackTop = -1;
    RBNode* current = kok;

    while (current != NULL || stackTop != -1) {
        while (current != NULL) {
            stack[++stackTop] = current;
            current = current->sol;
        }

        current = stack[stackTop--];

        if (!ziyaretEdilen[current->kullanici->kimlik]) {
            dfsToplulukTespit(current, ++toplulukSayaci);
        }

        current = current->sag;
    }
}

// Belirli bir topluluğun üyelerini yazdır
void toplulukYazdir(int toplulukID) {
    printf("Topluluk %d Uyeleri:\n", toplulukID);

    RBNode* stack[MAX_KULLANICI];
    int stackTop = -1;
    RBNode* current = kok;

    while (current != NULL || stackTop != -1) {
        while (current != NULL) {
            stack[++stackTop] = current;
            current = current->sol;
        }

        current = stack[stackTop--];

        if (current->kullanici->toplulukID == toplulukID) {
            printf("%d ", current->kullanici->kimlik);
        }

        current = current->sag;
    }
    printf("\n");
}

// Tüm toplulukları yazdır
void tumTopluluklariYazdir() {
    printf("\n--- Topluluk Tespit Sonuclari ---\n");
    printf("Toplam %d topluluk bulundu:\n", toplulukSayaci);

    for (int i = 1; i <= toplulukSayaci; i++) {
        toplulukYazdir(i);
    }
}


//--------------------------------------
// Ana Fonksiyon (Diğer fonksiyonlarla entegrasyon)
//--------------------------------------
int main() {
    // Önceki kodun aynısı
    dosyaOku("veriseti.txt");

    // Örnek veriler
    kullaniciBulVeyaOlustur(101);
    kullaniciBulVeyaOlustur(102);
    kullaniciBulVeyaOlustur(103);
    kullaniciBulVeyaOlustur(104);
    kullaniciBulVeyaOlustur(162);
    kullaniciBulVeyaOlustur(163);
    kullaniciBulVeyaOlustur(164);
    kullaniciBulVeyaOlustur(184);
    kullaniciBulVeyaOlustur(122);

    arkadasEkle(kullaniciBulVeyaOlustur(122), kullaniciBulVeyaOlustur(101));
    arkadasEkle(kullaniciBulVeyaOlustur(122), kullaniciBulVeyaOlustur(103));
    arkadasEkle(kullaniciBulVeyaOlustur(104), kullaniciBulVeyaOlustur(103));
    arkadasEkle(kullaniciBulVeyaOlustur(184), kullaniciBulVeyaOlustur(101));
    arkadasEkle(kullaniciBulVeyaOlustur(184), kullaniciBulVeyaOlustur(102));
    arkadasEkle(kullaniciBulVeyaOlustur(162),kullaniciBulVeyaOlustur(163));

    printf("--- 2 mesafedeki arkadaslar ---\n");
    belirliMesafedekiArkadaslariBul(101, 2);

    printf("--- Ortak arkadaslar ---\n");
    ortakArkadaslariBul(101, 102);

    printf("--- Etki alani ---\n");
    etkiAlaniniHesapla(101);

    // Topluluk tespitini çalıştır
    topluluklariBelirle();
    tumTopluluklariYazdir();

    agaciTemizle(kok);
    return 0;
}