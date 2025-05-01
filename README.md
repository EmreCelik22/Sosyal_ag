# SOSYAL AĞ ANALİZ SİSTEMİ

 ## 🔧 Özellikler

- Kullanıcıları Kırmızı-Siyah Ağaç kullanarak verimli bir şekilde ekler.
- Kullanıcılar arası arkadaşlık ilişkilerini graf yapısıyla temsil eder.
- Giriş verilerini `.txt` dosyasından okuyarak işlemleri otomatik yapar.
- Kullanıcıları ve sosyal bağlantılarını analiz edebilir.
 
 ## Bu program ne işe yarar
  Bu program, kullanıcılar arasındaki ilişkileri Red-Black Tree ve Graf yapılarıyla modelleyerek:
  - Arkadaşlık ilişkilerini yönetir
  - Topluluk tespiti yapar
  - Etki alanı analizi gerçekleştirir

 
##  Veri Yapıları:
  ### 1. Red-Black Tree
  Kullanıcıların kimlik numaralarına göre düzenli bir şekilde saklanması için kullanılıyor.
  Bu yapı sayesinde kullanıcılar kolay bir şekilde hızlıca eklenebiliyor kimliğe gore hızlıca arama yapılabiliyor.
  dengeli bir ikili arama ağacı olduğu için bu yapı kullanılıyor
  ### 2. Graf 
  Kullanıcılar arası ilişkilerin temsilinde kullanılıyor.
  Graf yapısı ile birlikte DFS ile topluluk tespiti yapılabilir ve
  ortak arkadaş bulma, mesafeye göre arkadaş önerme gibi analizler yapılabilir.

## NASIL ÇALIŞTIRILIR? 
Programı çalıştırmak için bir C dili derleyicisine ihtiyacınız olacak.

### Tavsiye Edilen Araçlar:
İnternete bağlı olmadan kullanılabilecek çevrimiçi derleyici: <a href="https://www.onlinegdb.com/online_c_compiler">Online C Compiler</a>
 
## Geliştirme Ortamı

- **Dil:** C
- **Geliştirici Araçları:** GCC derleyicisi
- **İşletim Sistemi:** Çoğu işletim sisteminde (Windows, Linux, MacOS) çalışır.
