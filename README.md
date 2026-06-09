# Boton de Emergencia Doctrinaria( B.E.D.)

Guía de instalación y diagnóstico para utilizar una placa Waveshare PCM5122 Audio HAT en Raspberry Pi OS.

## Hardware utilizado

* Raspberry Pi Zero
* Waveshare PCM5122 Audio HAT
* Raspberry Pi OS Bookworm
* Salida de audio por Jack 3.5 mm o RCA

---

# Instalación

## 1. Deshabilitar el audio onboard

Editar:

```bash
sudo nano /boot/firmware/config.txt
```

Agregar:

```ini
dtparam=audio=off
dtoverlay=hifiberry-dac
```

Guardar y reiniciar:

```bash
sudo reboot
```

---

## 2. Instalar utilidades ALSA

```bash
sudo apt update
sudo apt install alsa-utils -y
```

---

## 3. Verificar detección del DAC

```bash
aplay -l
```

Salida esperada:

```text
card 0: vc4hdmi
card 1: sndrpihifiberry
```

La aparición de `sndrpihifiberry` confirma que el driver se cargó correctamente.

---

# Pruebas de audio

## Generar tono de prueba

```bash
speaker-test -D hw:1,0 -c 2 -t sine
```

Salida esperada:

```text
0 - Front Left
1 - Front Right
```

---

## Reproducir archivo WAV

```bash
aplay -D hw:1,0 archivo.wav
```

Ejemplo:

```bash
aplay -D hw:1,0 /usr/share/sounds/alsa/Front_Center.wav
```

---

# Diagnóstico

## Verificar que el DAC fue detectado

```bash
aplay -l
```

Debe aparecer:

```text
sndrpihifiberry
```

---

## Verificar señales I2S

Mientras se ejecuta:

```bash
speaker-test -D hw:1,0 -c 2 -t sine
```

Medir con osciloscopio:

| GPIO   | Señal | Valor esperado          |
| ------ | ----- | ----------------------- |
| GPIO18 | BCLK  | ~1.536 MHz              |
| GPIO19 | LRCLK | 48 kHz                  |
| GPIO21 | DATA  | Datos digitales activos |

Si las tres señales están presentes, la Raspberry está enviando audio digital correctamente.

---

## Verificar pin XSMT

Medir el pin XSMT del PCM5122.

Valor esperado:

```text
3.3 V
```

Si está en bajo, el DAC permanece muteado.

---

## Verificar salida analógica

Conectar el osciloscopio a:

* RCA izquierdo
* RCA derecho
* Jack de audio

Durante la ejecución de:

```bash
speaker-test -D hw:1,0 -c 2 -t sine
```

Debe observarse una señal senoidal de aproximadamente 440 Hz.

---

# Diagnóstico realizado

Se verificó correctamente:

* Driver cargado.
* DAC detectado por ALSA.
* Reproducción ALSA funcional.
* BCLK presente (~1.536 MHz).
* LRCLK presente (48 kHz).
* DATA presente.
* XSMT = 3.3 V.

Esto confirma que la Raspberry está transmitiendo correctamente el flujo I2S hacia el DAC.

Si no existe señal analógica en las salidas RCA o Jack pese a estas verificaciones, las posibles causas son:

* Falla de alimentación interna del PCM5122.
* Falla de la etapa analógica de salida.
* Defecto de fabricación del módulo.
* Problema en el circuito de filtrado/salida analógica.

---

# Comandos útiles

Listar dispositivos de audio:

```bash
aplay -l
```

Listar dispositivos ALSA:

```bash
aplay -L
```

Generar tono:

```bash
speaker-test -D hw:1,0 -c 2 -t sine
```

Reproducir WAV:

```bash
aplay -D hw:1,0 archivo.wav
```

Escanear dispositivos I2C:

```bash
sudo apt install i2c-tools -y
i2cdetect -y 1
```
