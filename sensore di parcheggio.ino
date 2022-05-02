#include <Fuzzy.h>
// Istanzio le variabili relative ai pin della scheda
const int buzzPin=4;	// Buzzer
const int trigPin=3;	// Sensore distanza
const int echoPin=2;	// Sensore distanza
long durata;
int distanza;
// Istanzio un oggetto Fuzzy
Fuzzy *fuzzy = new Fuzzy();

void setup()
{
	// Set del trigPin come Output
	pinMode(trigPin, OUTPUT);
	// Set del echoPin come Input
	pinMode(echoPin, INPUT);
	// Set del Serial output
	Serial.begin(9600);
	// Set del random seed
	randomSeed(analogRead(0));

	// Istanzio l'oggetto FuzzyInput per la distanza
	FuzzyInput *distanza = new FuzzyInput(1);
	// Istanzio l'oggetto FuzzySet per la distanza vicina
	FuzzySet *vicina = new FuzzySet(0, 20, 20, 40);
	// Istanzio l'oggetto FuzzySet per la distanza media
	FuzzySet *media = new FuzzySet(30, 50, 50, 70);
	// Istanzio l'oggetto FuzzySet per la distanza lontana
	FuzzySet *lontana = new FuzzySet(70, 80, 90, 100);
	// Includo i FuzzySet nel FuzzyInput
	distanza->addFuzzySet(vicina);
	distanza->addFuzzySet(media);
	distanza->addFuzzySet(lontana);
	// Includo il FuzzyInput nel Fuzzy
	fuzzy->addFuzzyInput(distanza);

	// Istanzio l'oggetto FuzzyOutput per il tempo tra un intervallo e l'altro
	FuzzyOutput *intervallo = new FuzzyOutput(1);
	// Istanzio l'oggetto FuzzySet per l'intervallo lungo
	FuzzySet *lungo = new FuzzySet(700, 800, 800, 1000);
	// Istanzio l'oggetto FuzzySet per l'intervallo moderato
	FuzzySet *moderato = new FuzzySet(300, 500, 500, 700);
	// Istanzio l'oggetto FuzzySet per l'intervallo breve
	FuzzySet *breve = new FuzzySet(0, 0, 0, 300);
	// Includo i FuzzySet nel FuzzyOutput
	intervallo->addFuzzySet(lungo);
	intervallo->addFuzzySet(moderato);
	intervallo->addFuzzySet(breve);
	// Includo il FuzzyOutput nel Fuzzy
	fuzzy->addFuzzyOutput(intervallo);

	// Creo la FuzzyRule R1 "IF distanza IS vicina THEN intervallo IS breve"
	// Istanzio l'oggetto FuzzyRuleAntecedent
	FuzzyRuleAntecedent *ifDistanzaVicina = new FuzzyRuleAntecedent();
	// Creo un FuzzyRuleAntecedent con un solo FuzzySet
	ifDistanzaVicina->joinSingle(vicina);
	// Istanzio un oggetto FuzzyRuleConsequent
	FuzzyRuleConsequent *thenIntervalloBreve = new FuzzyRuleConsequent();
	// Includo un FuzzySet al FuzzyRuleConsequent
	thenIntervalloBreve->addOutput(breve);
	// Istanzio un oggetto FuzzyRule
	FuzzyRule *fuzzyRule01 = new FuzzyRule(1, ifDistanzaVicina, thenIntervalloBreve);
	// Includo la FuzzyRule in Fuzzy
	fuzzy->addFuzzyRule(fuzzyRule01);

	// Creo la FuzzyRule R2 "IF distanza IS media THEN intervallo IS moderato"
	// Istanzio l'oggetto FuzzyRuleAntecedent
	FuzzyRuleAntecedent *ifDistanzaMedia = new FuzzyRuleAntecedent();
	// Creo un FuzzyRuleAntecedent con un solo FuzzySet
	ifDistanzaMedia->joinSingle(media);
	// Istanzio un oggetto FuzzyRuleConsequent
	FuzzyRuleConsequent *thenIntervalloModerato = new FuzzyRuleConsequent();
	// Includo un FuzzySet al FuzzyRuleConsequent
	thenIntervalloModerato->addOutput(moderato);
	// Istanzio un oggetto FuzzyRule
	FuzzyRule *fuzzyRule02 = new FuzzyRule(2, ifDistanzaMedia, thenIntervalloModerato);
	// Includo la FuzzyRule in Fuzzy
	fuzzy->addFuzzyRule(fuzzyRule02);

	// Creo la FuzzyRule R3 "IF distanza IS lontana THEN intervallo IS lungo"
	// Istanzio l'oggetto FuzzyRuleAntecedent
	FuzzyRuleAntecedent *ifDistanzaLontana = new FuzzyRuleAntecedent();
	// Creo un FuzzyRuleAntecedent con un solo FuzzySet
	ifDistanzaLontana->joinSingle(lontana);
	// Istanzio un oggetto FuzzyRuleConsequent
	FuzzyRuleConsequent *thenIntervalloLungo = new FuzzyRuleConsequent();
	// Includo un FuzzySet al FuzzyRuleConsequent
	thenIntervalloLungo->addOutput(lungo);
	// Istanzio un oggetto FuzzyRule
	FuzzyRule *fuzzyRule03 = new FuzzyRule(3, ifDistanzaLontana, thenIntervalloLungo);
	// Includo la FuzzyRule in Fuzzy
	fuzzy->addFuzzyRule(fuzzyRule03);
}

void loop()
{
	// Azzero i Pin
	digitalWrite(buzzPin, LOW);
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	// Imposto il trigPin su HIGH per 10 microsecondi
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	// Leggo l' echoPin, ritorna il tempo che impiega l'onda per viaggiare in microsecondi
	durata = pulseIn(echoPin, HIGH);
	// Calcolo la distanza
	distanza = durata * 0.034 / 2;
	// Se la distanza è inferiore o uguale a 100cm parte il sensore
	if(distanza<=100){
		// Stampo su monitor seriale
		Serial.print("Distanza: ");
		Serial.print(distanza);
		Serial.println(" cm");
		// Setto la distanza come imput per il fuzzy
		fuzzy->setInput(1, distanza);
		// Parte la fuzzificazione
		fuzzy->fuzzify();
		// Parte la defuzzificazione
		float output = fuzzy->defuzzify(1);
		// Attendo che passi l'intervallo per il prossimo segnale sonoro
		delay(output);
		// Attivo il buzzer
		digitalWrite(buzzPin, HIGH);
		
		// Stampo su monitor seriale
		Serial.print("Intervallo: ");
		Serial.print(output/1000);
		Serial.println(" sec");
	}
	Serial.flush();
}