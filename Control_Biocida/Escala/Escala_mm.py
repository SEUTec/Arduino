#!/usr/bin/env python
#coding:utf-8
# Author:  Sebas
# Purpose: Creació d'una Escala Graduala per Nivells
# Created: 01/06/2023
# El SVG que queda no està a escala en mm ni de conya ?
# Tenim de definir una mida al fitxer per a que InkScape respecte les dimensions
# Y al imprimir quede el dibuix amb els mm especificats.
# Que te de ser una mica més gran que l'area necessaria per al dibuix.
# dwg = svgwrite.Drawing('Escala_mm.svg', size=(50*mm, (mm_Inicial_Final*1.1)*mm))
# o simplement les mides l'un full DIN A4.
# dwg = svgwrite.Drawing('Escala_mm.svg', size=(210*mm, 297*mm))

import svgwrite
from svgwrite import cm, mm

# Parametres Primer Tram de 10 Divisions
Valor_Inicial = 0  # Primer Tram
print("Valor_Inicial", Valor_Inicial)
Valor_Final = 100  # Primer Tram
print("Valor_Final", Valor_Final)
Unitats_Mesura = "Litros"
print("Unitats_Mesura", Unitats_Mesura)

#mm_Inicial_Final = 112.5        # Segons mides del plano del tank
mm_Inicial_Final = 115.5        # Segons les nostres mesures
print("mm_Inicial_Final", mm_Inicial_Final)
Unitats_Ditancia = "mm"

Divisions = 10
print("Divisions", Divisions)

# Distancia entre Divicions
mm_Div = mm_Inicial_Final / Divisions
print("mm_Div", mm_Div)
dValor = (Valor_Final - Valor_Inicial) / Divisions
print("dValor", dValor)

Alt_Text = 5   # Altura text en mm
print("Alt_Text", Alt_Text)
Num_pos_x = 17
Line_end = 15
Line_crt = 2  # Longitud linies en mm ?
Line_mj = 4
Line_lng = 8

Line_pos_y = 10  # Posició Y de la Primera línea

# Tamaño de Hoja DIN A4 en mm
dwg = svgwrite.Drawing('Escala_mm.svg', size=(210*mm, 297*mm))

hlines = dwg.add(dwg.g(id='hlines', stroke='black', stroke_width="3"))
digits = dwg.add(dwg.g(font_size=Alt_Text*mm, style='font-weight:bold'))

def Tram():
    # Primer Valor
    i = 0
    hlines.add(dwg.line(start=((Line_end - Line_lng) * mm, (Line_pos_y + i * mm_Div) * mm),
                        end=(Line_end * mm, (Line_pos_y + i * mm_Div) * mm)))
    # Primera part del primer Tram
    for i in range (1, int(Divisions)+1):
        # Valor Mig
        if (i == int(Divisions / 2)):
            hlines.add(dwg.line(start=((Line_end - Line_mj) * mm, (Line_pos_y+i*mm_Div) * mm), end=(Line_end * mm, (Line_pos_y+i*mm_Div) * mm)))
        # Ultim valor
        if (i == int(Divisions)):
            hlines.add(dwg.line(start=((Line_end - Line_lng) * mm, (Line_pos_y+i*mm_Div) * mm), end=(Line_end * mm, (Line_pos_y+i*mm_Div) * mm)))
        else:  # Valor intermitjos normal
            hlines.add(dwg.line(start=((Line_end - Line_crt) * mm, (Line_pos_y+i*mm_Div) * mm), end=(Line_end * mm, (Line_pos_y+i*mm_Div) * mm)))
        digits.add(dwg.text(int(Valor_Inicial + i * dValor), insert=(Num_pos_x * mm, (Line_pos_y + Alt_Text/3 + i * mm_Div) * mm)))

    # Segona part del Primer tram
    for i in range (Divisions, 2*int(Divisions)+1):
        # Valor Mig
        if (i == Divisions + int(Divisions / 2)):
            hlines.add(dwg.line(start=((Line_end - Line_mj) * mm, (Line_pos_y+i*mm_Div) * mm), end=(Line_end * mm, (Line_pos_y+i*mm_Div) * mm)))
        # Ultim valor
        if (i == 2*int(Divisions)):
            hlines.add(dwg.line(start=((Line_end - Line_lng) * mm, (Line_pos_y+i*mm_Div) * mm), end=(Line_end * mm, (Line_pos_y+i*mm_Div) * mm)))
        else:  # Valor intermitjos normal
            hlines.add(dwg.line(start=((Line_end - Line_crt) * mm, (Line_pos_y+i*mm_Div) * mm), end=(Line_end * mm, (Line_pos_y+i*mm_Div) * mm)))
        if (i != Divisions ):
            digits.add(dwg.text(int(Valor_Inicial + i * dValor), insert=(Num_pos_x * mm, (Line_pos_y + Alt_Text/3 + i * mm_Div) * mm)))

# Primer Trap
i = 0
digits.add(dwg.text(int(Valor_Inicial + i * dValor), insert=(Num_pos_x * mm, (Line_pos_y + Alt_Text/3 + i * mm_Div) * mm)))
Tram()

#d_Num_pos_x = 76
#d_Line_end = 20

d_Num_pos_x = 27
d_Line_end = 27

Valor_Inicial = 200 # Segon Tram
Valor_Final = 300
Num_pos_x = Num_pos_x + d_Num_pos_x
Line_end = Line_end + d_Line_end
Tram()

Valor_Inicial = 400 # Segon Tram
Valor_Final = 500
Num_pos_x = Num_pos_x + d_Num_pos_x
Line_end = Line_end + d_Line_end
Tram()

Valor_Inicial = 600 # Segon Tram
Valor_Final = 700
Num_pos_x = Num_pos_x + d_Num_pos_x
Line_end = Line_end + d_Line_end
Tram()

Valor_Inicial = 800 # Segon Tram
Valor_Final = 900
Num_pos_x = Num_pos_x + d_Num_pos_x
Line_end = Line_end + d_Line_end
Tram()

Valor_Inicial = 1000 # Segon Tram
Valor_Final = 1100
Num_pos_x = Num_pos_x + d_Num_pos_x
Line_end = Line_end + d_Line_end
Tram()

Valor_Inicial = 1200 # Segon Tram
Valor_Final = 1300
Num_pos_x = Num_pos_x + d_Num_pos_x
Line_end = Line_end + d_Line_end
Tram()

dwg.save()