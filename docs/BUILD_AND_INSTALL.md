# Construcción e instalación de prueba — SX500 IS 1.00D

## Qué genera este proyecto

El workflow de GitHub Actions compila CHDK para **PowerShot SX500 IS / firmware Canon 1.00D** usando una instantánea fijada del repositorio público de CHDK y GNU Arm Embedded 10.3.2021.10.

El perfil recorta componentes opcionales de CHDK. Conserva las rutinas Canon de enfoque, exposición, disparo y zoom. No modifica tareas DryOS, direcciones de memoria, el tiempo de espera del contador de archivo ni el guardado RAW. JPEG queda habilitado.

**No es un firmware Canon ni reemplaza el firmware interno.** El paquete está pensado para cargarse desde la tarjeta SD. El puerto SX500 sigue clasificado como BETA; sus notas indican que la escritura de archivos no está probada y que la calibración DNG no está terminada. Por eso el resultado es un paquete de prueba, no una versión validada en cámara.

## Obtener el paquete compilado

1. Abrir la pestaña **Actions** del repositorio.
2. Entrar en la ejecución **Build SX500IS 1.00D lean CHDK test package**.
3. Descargar el artefacto **CHDK-SX500IS-100D-LEAN-TEST**.
4. Dentro del ZIP del artefacto están:
   - `CHDK_SX500IS_100D_LEAN_TEST_FULL.zip`: distribución completa para la SD.
   - `CHDK_SX500IS_100D_LEAN_TEST_SMALL.zip`: distribución reducida.
   - `BUILD_INFO.txt` y `SHA256SUMS.txt`.

El workflow comprueba que el paquete contenga `DISKBOOT.BIN` y módulos CHDK, y que no contenga `PS.FI2` ni `PS.FIR`. Por lo tanto, **no** se instala desde “Firmware Update”; se usa arranque desde tarjeta preparada como booteable.

## Preparar e instalar en la SD

1. Usar preferentemente una tarjeta SD de prueba. Copiar primero todas las fotos y hacer una copia de respaldo de toda la tarjeta actual.
2. Extraer el contenido de `CHDK_SX500IS_100D_LEAN_TEST_FULL.zip` directamente en la raíz de la tarjeta, conservando las carpetas. `DISKBOOT.BIN` debe quedar en la raíz y debe existir la carpeta `CHDK`.
3. Preparar la tarjeta como booteable con una herramienta de instalación CHDK compatible, como STICK o CHIMP. No improvisar el sector de arranque ni usar herramientas de particionado genéricas.
4. Insertar la tarjeta con el seguro lateral en posición **LOCK** y encender la cámara normalmente. El CHDK debería mostrar su pantalla de inicio.
5. Para iniciar sin CHDK, apagar, mover el seguro a **UNLOCK** y volver a encender.

Si la herramienta propone formatear o reparticionar, detenerse y confirmar primero que la copia de seguridad está completa. No usar una tarjeta que contenga la única copia de fotos.

## Activar RAW

En el puerto SX500, el botón de reproducción `[>]` entra en el modo ALT de CHDK.

1. Pulsar `[>]` para entrar en ALT.
2. Abrir el menú CHDK con `MENU`.
3. Ir a los ajustes de RAW y activar **Save RAW**.
4. Dejar **DNG desactivado** en esta primera prueba, porque las notas del puerto indican que su calibración DNG no está terminada.
5. JPEG seguirá guardándose como respaldo. CHDK Save RAW añade el archivo RAW; no sustituye automáticamente al JPEG.

## Prueba segura inicial

- Hacer las primeras pruebas con batería bien cargada y una tarjeta de prueba.
- Tomar una foto JPEG+RAW, revisar que ambos archivos existan y que el RAW se pueda abrir con una herramienta compatible.
- Comprobar enfoque con media presión, disparo con presión completa, zoom, reproducción y controles Canon.
- Si aparece un bloqueo, error de lente o error de escritura, apagar, desbloquear la tarjeta y arrancar sin CHDK.
- No instalar archivos `PS.FI2`/`PS.FIR` ni actualizar el firmware interno.

## Qué NO promete esta compilación

- No garantiza autofocus más rápido: el autofocus sigue siendo el de Canon.
- No garantiza menor tiempo entre fotos: escribir un RAW de aproximadamente 24,8 MB puede alargar la disponibilidad para el siguiente disparo.
- No elimina la generación JPEG.
- No cambia el tiempo de sincronización de archivo ni libera anticipadamente el buffer RAW.
- No es una compilación probada físicamente en una SX500 IS 1.00D.
