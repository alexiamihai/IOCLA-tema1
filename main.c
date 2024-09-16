#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

void get_operations(void **operations);

int main(int argc, char const *argv[])
{
	// am declarat vectorul de operatii si prin intermediul unui pointer de
	// tip void ** am apelat functia get_operations
	void (*operation_pointer[8])();
	void **operation;
	operation = (void **)operation_pointer;
	get_operations(operation);
	int nr_senzori, i = 0, tip_senzor, j = 0, x, k, ok = 0, l, eroare;
	char comanda[100], index[5], aux[100];
	FILE *f = fopen(argv[1], "rb");
	fread(&nr_senzori, sizeof(int), 1, f);
	/*
	pentru a sorta mai usor vectorul de senzori, astfel incat cei de tip
	pmu sa fie primii din vector, am folosit pentru citire 2 vectori de tip
	sensor *, pentru care am alocat memorie
	 */
	sensor *v = (sensor *)malloc(nr_senzori * sizeof(sensor));
	sensor *v_tire = (sensor *)malloc(nr_senzori * sizeof(sensor));
	/*
	i reprezinta numarul de elemente al vectorului v, adica numarul de
	senzori PMU, iar j numarul de elemente al vectorului v_tire, adica
	numarul de senzori TIRE
	astfel, am citit senzori pana cand am ajuns la numarul total
	*/
	while (i + j < nr_senzori)
	{
		fread(&tip_senzor, sizeof(int), 1, f);
		// am retinut in vectorul v_tire componentele fiecarui senzor de tip
		// TIRE
		if (tip_senzor == 0)
		{
			v_tire[j].sensor_type = TIRE;
			// am alocat memorie pentru datele senzorului, folosindu-ma de
			// structura tire_sensor
			v_tire[j].sensor_data = malloc(sizeof(tire_sensor));
			fread(v_tire[j].sensor_data, sizeof(tire_sensor), 1, f);
			fread(&v_tire[j].nr_operations, sizeof(int), 1, f);
			// am alocat memorie pentru un numar de operatii egal cu numarul
			// citit anterior din fisier si apoi am citit fiecare operatie
			v_tire[j].operations_idxs = malloc(v_tire[j].nr_operations *
											   sizeof(int));
			for (k = 0; k < v_tire[j].nr_operations; k++)
			{
				fread(&v_tire[j].operations_idxs[k], sizeof(int), 1, f);
			}
			j++;
		}
		// am retinut in vectorul v componentele fiecarui senzor de tip PMU
		// si am repetat acelasi procedeu, folosit si pentru senzorii TIRE
		else
		{
			v[i].sensor_type = PMU;
			v[i].sensor_data = malloc(sizeof(power_management_unit));
			fread(v[i].sensor_data, sizeof(power_management_unit), 1, f);
			fread(&v[i].nr_operations, sizeof(int), 1, f);
			v[i].operations_idxs = malloc(v[i].nr_operations * sizeof(int));
			for (k = 0; k < v[i].nr_operations; k++)
			{
				fread(&v[i].operations_idxs[k], sizeof(int), 1, f);
			}
			i++;
		}
	}
	// am copiat apoi senzorii de tip TIRE din vectorul v_tire in v
	k = 0;
	while (k < j)
	{
		v[i] = v_tire[k];
		i++;
		k++;
	}
	// in variabila comanda am retinut initial fiecare comanda din fisier
	while (fgets(comanda, 100, stdin))
	{
		// am folosit variabila ok pentru a verifica daca comanda citita
		// contine spatii
		ok = 0;
		// m-am asigurat ca ultimul caracter al comenzii nu este '\n'
		comanda[strlen(comanda) - 1] = '\0';
		// am verificat daca exista spatii in comanda
		for (l = 0; l < strlen(comanda); l++)
		{
			if (comanda[l] == ' ')
			{
				ok = 1;
			}
		}
		// am vrut sa pastrez in string ul comanda doar textul comenzii
		strcpy(aux, strtok(comanda, " "));
		if (ok == 1)
		{
			/*
			in cazul in care comanda contine un spatiu, inseamna ca
			aceasta are un index, pe care l-am pastrat in variabila
			index, iar apoi l-am transformat din caracter in cifra
			*/
			strcpy(index, strtok(NULL, " "));
			x = atoi(index);
			// in cazul in care indexul nu este valid, am afisat acest lucru
			if (x > nr_senzori || x < 0)
			{
				printf("Index not in range!\n");
			}
			else
			{
				// cazul in care a fost citita comanda print
				if (strcmp(comanda, "print") == 0)
				{
					// in functie de tipul senzorului cu indexul dat se afiseaza
					// datele acestuia
					if (v[x].sensor_type == TIRE)
					{
						printf("Tire Sensor\n");
						// am facut cast la tire_sensor * pentru a putea
						// accesa datele
						printf("Pressure: %.2lf\n",
							   ((tire_sensor *)v[x].sensor_data)->pressure);
						printf("Temperature: %.2lf\n",
							   ((tire_sensor *)v[x].sensor_data)->temperature);
						printf("Wear Level: %d%%\n",
							   ((tire_sensor *)v[x].sensor_data)->wear_level);
						if (((tire_sensor *)v[x].sensor_data)->performace_score
						 == 0)
						{
							printf("Performance Score: Not Calculated\n");
						}
						else
						{
							printf("Performance Score: %d\n",
								   ((tire_sensor *)v[x].sensor_data)
								   ->performace_score);
						}
					}
					else
					{
						printf("Power Management Unit\n");
						// am facut cast la power_management_unit * pentru a
						// putea accesa datele
						printf("Voltage: %.2lf\n",
							   ((power_management_unit *)v[x].sensor_data)
							   ->voltage);
						printf("Current: %.2lf\n",
							   ((power_management_unit *)v[x].sensor_data)
							   ->current);
						printf("Power Consumption: %.2lf\n",
							   ((power_management_unit *)v[x].sensor_data)
								   ->power_consumption);
						printf("Energy Regen: %d%%\n",
							   ((power_management_unit *)v[x].sensor_data)
								   ->energy_regen);
						printf("Energy Storage: %d%%\n",
							   ((power_management_unit *)v[x].sensor_data)
								   ->energy_storage);
					}
				}
				// cazul in care a fost citita comanda analyze
				if (strcmp(comanda, "analyze") == 0)
				{
					// pentru senzorul cu indexul dat se efectueaza operatiile
					// retinute, apelate prin intermediul vectorului
					for (l = 0; l < v[x].nr_operations; l++)
					{
						(*operation_pointer[v[x].operations_idxs[l]])
						(v[x].sensor_data);
					}
				}
			}
		}
		else
		{
			// cazul in care a fost citita comanda exit
			if (strcmp(comanda, "exit") == 0)
			{
				// se opreste citirea si se opreste programul
				break;
			}
			// cazul in care a fost citita comanda clear
			if (strcmp(comanda, "clear") == 0)
			{
				// se verifica fiecare senzor din v
				for (x = 0; x < nr_senzori; x++)
				{
					// variabila eroare va semnala existenta unui senzor cu
					// valori eronate
					eroare = 0;
					// pentru fiecare tip de senzor am verificat daca datele
					// sale indeplinesc conditiile
					if (v[x].sensor_type == TIRE)
					{
						if (((tire_sensor *)v[x].sensor_data)->pressure < 19 ||
						 ((tire_sensor *)v[x].sensor_data)->pressure > 28)
						{
							eroare = 1;
						}
						if (((tire_sensor *)v[x].sensor_data)->temperature < 0
						|| ((tire_sensor *)v[x].sensor_data)->temperature > 120)
						{
							eroare = 1;
						}
						if (((tire_sensor *)v[x].sensor_data)->wear_level < 0 ||
						 ((tire_sensor *)v[x].sensor_data)->wear_level > 100)
						{
							eroare = 1;
						}
					}
					else
					{
						if (((power_management_unit *)v[x].sensor_data)
									->voltage < 10 ||
							((power_management_unit *)
								 v[x]
									 .sensor_data)
									->voltage > 20)
						{
							eroare = 1;
						}
						if (((power_management_unit *)v[x].sensor_data)
									->current < -100 ||
							((power_management_unit *)
								 v[x]
									 .sensor_data)
									->current > 100)
						{
							eroare = 1;
						}
						if (((power_management_unit *)v[x].sensor_data)
									->power_consumption < 0 ||
							((power_management_unit *)
								 v[x]
									 .sensor_data)
									->power_consumption > 1000)
						{
							eroare = 1;
						}
						if (((power_management_unit *)v[x].sensor_data)
									->energy_regen < 0 ||
							((power_management_unit *)
								 v[x]
									 .sensor_data)
									->energy_regen > 100)
						{
							eroare = 1;
						}
						if (((power_management_unit *)v[x].sensor_data)
									->energy_storage < 0 ||
							((power_management_unit *)
								 v[x]
									 .sensor_data)
									->energy_storage > 100)
						{
							eroare = 1;
						}
					}
					// daca senzorul are valori eronate, atunci este eliminat
					// din vector
					if (eroare == 1)
					{
						// am eliberat memoria alocata pentru componentele lui
						free(v[x].sensor_data);
						free(v[x].operations_idxs);
						// am sters senzorul si am copiat componentele
						// urmatorilor senzori peste acesta
						for (j = x; j < nr_senzori - 1; j++)
						{
							v[j] = v[j + 1];
						}
						// indicele senzorului scade, pentru a verifica daca
						// si valorile noului vector cu indexul x sunt corecte
						x--;
						// numarul de senzori scade dupa eliminare
						nr_senzori--;
						// am realocat memoria necesara pentru vector, dupa
						// eliminare
						v = realloc(v, nr_senzori * sizeof(sensor));
					}
				}
			}
		}
	}
	// am eliberat memoria folosita pentru componentele vectorului
	for (i = 0; i < nr_senzori; i++)
	{
		free(v[i].sensor_data);
		free(v[i].operations_idxs);
	}
	// am eliberat vectorii folositi
	free(v);
	free(v_tire);
	fclose(f);
	return 0;
}
