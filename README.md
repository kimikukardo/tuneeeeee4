# Realtune

Plugin de autotune (VST3) con interfaz inspirada en el Omnitrix, hecho con JUCE.

## Como suena vs. Antares

- **Deteccion de tono:** algoritmo YIN (`PitchDetector`), pensado para voz monofonica.
- **Correccion:** `PitchShifter` usa una tecnica tipo PSOLA (recorta "granos" de audio del
  historial reciente y los reacomoda con la nueva separacion temporal). Esto conserva mejor
  el timbre que un simple cambio de velocidad de reproduccion.
- **La clave del sonido "no robotico":** el pitch corregido *desliza* hacia la nota objetivo
  en vez de saltar de golpe (eso es lo que hace sonar robotico a Auto-Tune clasico en modo
  duro). La perilla **Speed** controla esa velocidad de deslizamiento: abajo = mas natural,
  arriba = correccion casi instantanea.

## Controles

- **Key**: tonica (C, C#, D...).
- **Scale**: escala a la que se corrige (cromatica, mayor, menor natural, menor armonica,
  pentatonica mayor/menor).
- **Speed**: velocidad de correccion (ver arriba).
- **Mix**: mezcla entre la señal original y la corregida.

## Interfaz

Se renombro de "Mikutune" a "Realtune". El diseño visual sigue la imagen del Omnitrix que
compartiste: chasis negro, 4 picos grises con puntas blancas redondeadas (ahi van las
perillas de Speed y Mix), anillo central con el simbolo en cruz verde que muestra la nota
detectada en vivo, y 4 puntos verdes como los "tornillos" de la imagen. Si queres ajustar
algo mas del diseño o de los colores, decime y lo cambio.

## Como compilarlo en Windows (vos mismo, paso a paso)

No pude compilar ni escuchar esto yo mismo: este chat no tiene conexion a internet ni el
SDK de JUCE instalado, asi que es un primer borrador completo pero sin probar. Es normal
que al compilar aparezcan uno o dos errores menores para ajustar.

1. **Instalar Visual Studio 2022 Community** (gratis) con el componente "Desarrollo para
   el escritorio con C++".
2. **Instalar CMake** (cmake.org, version 3.22 o mas nueva).
3. **Descargar JUCE** desde https://juce.com/get-juce (version con codigo fuente, no el
   instalador de Projucer solo). Descomprimilo.
4. Armar esta estructura de carpetas (la carpeta `JUCE` como hermana de `Realtune`):
   ```
   MisProyectos/
     JUCE/            <- lo que descargaste en el paso 3
     Realtune/         <- esta carpeta
   ```
5. Abrir una terminal dentro de `Realtune/` y correr:
   ```
   cmake -B build -G "Visual Studio 17 2022"
   cmake --build build --config Release
   ```
6. El VST3 queda en `build/Realtune_artefacts/Release/VST3/Realtune.vst3`, y con
   `COPY_PLUGIN_AFTER_BUILD` deberia copiarse solo a la carpeta de VST3 de Windows
   (`C:\Program Files\Common Files\VST3`). Tambien se genera una version Standalone para
   probarlo sin abrir un DAW.

## Compilarlo en GitHub (sin instalar nada en tu PC)

Esta carpeta ya incluye `.github/workflows/build.yml`, que compila el plugin en un
servidor de GitHub cada vez que subis cambios, sin que vos tengas que instalar Visual
Studio ni nada.

1. Crea una cuenta gratis en https://github.com (si no tenes una).
2. Crea un repositorio nuevo (puede ser publico, "New repository").
3. Entra al repo y usa "Add file" -> "Upload files". Arrastra TODO el contenido de esta
   carpeta `Realtune` (los archivos sueltos, la carpeta `Source` y la carpeta `.github`).
   Importante: que `CMakeLists.txt` quede en la raiz del repo, no adentro de una
   subcarpeta "Realtune" dentro del repo.
4. Hace commit (el boton verde "Commit changes").
5. Anda a la pestaña **Actions** del repo. El workflow deberia arrancar solo apenas
   subis los archivos (si no, apreta "Run workflow").
6. Esperalo: la primera vez tarda bastante (compila JUCE entero), puede ser 10-20
   minutos. Cuando termina con un tilde verde, entra a ese run.
7. Abajo de todo vas a ver **Artifacts**: ahi esta `Realtune-VST3` (el plugin) y
   `Realtune-Standalone` (el .exe para probarlo sin DAW). Descargalos.
8. Descomprimi el zip de `Realtune-VST3` y copia la carpeta/archivo `Realtune.vst3` a
   `C:\Program Files\Common Files\VST3\`.

Cada vez que me pidas un cambio y te pase archivos nuevos, solo tenes que volver a subir
los archivos modificados al repo (paso 3) y GitHub lo recompila solo.

## Si algo no compila

Pegame el error exacto que te tira Visual Studio/CMake en el chat y lo arreglo. Para una
vuelta mas rapida de compilar-probar-corregir en tu propia maquina (sin tener que copiar y
pegar errores), otra opcion es usar Claude Code en tu PC: ahi puedo compilar, leer los
errores y corregirlos yo mismo en el momento.

## Notas / proximos pasos posibles

- El `PitchShifter` es una version simplificada de PSOLA; es un buen punto de partida pero
  puede necesitar ajustes de oido (duracion de los granos, suavizado) una vez que lo
  escuches.
- Por ahora es mono (entrada y salida de 1 canal), tipico para una pista de voz.
- AU (para Mac) no esta en el CMakeLists porque requiere compilarse en macOS con Xcode; si
  mas adelante tenes una Mac, se agrega facil.
