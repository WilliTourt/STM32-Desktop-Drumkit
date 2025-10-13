/**
 * @file cpp_main.cpp
 * @brief Main application logic for drumkit
 * 
 * This file implements the main application logic including:
 * - Pad initialization and configuration
 * - MIDI communication setup
 * - Main processing loop
 * - User interface updates
 * 
 * @author WilliTourt, willitourt@foxmail.com
 * @copyright Copyright (c) 2025 by WilliTourt
 * 
 * @note Comments are mostly written by AI
 * 
 * @CHANGELOG:
 * - 2025-09-14: V1 plan complete, file created
 * - 2025-10-13: V1.0.0 release
 * 
 */

#include "cpp_main.h"
#include "pad.h"
#include "midi.h"
#include "ui.h"

/**
 * @brief Hit threshold offset value
 * 
 * This offset is used to reduce interference possibility.
 * Higher values for more noisy environments, but may miss light hits.
 * Hit threshold is set as (stable_value + offset)
 */
#define HIT_THRESHOLD_OFFSET 310

/**
 * @brief Initialize all drum pads
 * 
 * Parameters for each pad:
 * @param adc_group ADC group (1-3)
 * @param adc_ch ADC channel
 * @param out_port GPIO port for output
 * @param out_pin GPIO pin for output 
 * @param pad_id Pad identifier
 * @param hit_threshold Trigger threshold (stable_value + offset)
 * @param upper_limit Maximum force value
 * @param force_curve Force mapping curve type
 */
// Initialize all drum pad instances
//  Pad insts:   adc_group , adc_ch, out_port          		 , out_pin           , pad_id         , hit_threshold                , upper_limit(MaxF), force_curve
Pad OpenHiHat 	(Pad::ADC_1, 0     , OPENHIHAT_OUT_GPIO_Port , OPENHIHAT_OUT_Pin , Pad::OpenHiHat , (1023 + HIT_THRESHOLD_OFFSET), 2084 			, Pad::CURVE_LINEAR);
Pad CloseHiHat	(Pad::ADC_1, 1     , CLOSEHIHAT_OUT_GPIO_Port, CLOSEHIHAT_OUT_Pin, Pad::CloseHiHat, (580  + HIT_THRESHOLD_OFFSET), 2330 			, Pad::CURVE_LINEAR);
Pad Crash	  	(Pad::ADC_1, 2     , CRASH_OUT_GPIO_Port	 , CRASH_OUT_Pin	 , Pad::Crash	  , (416  + HIT_THRESHOLD_OFFSET), 2801 			, Pad::CURVE_LINEAR);
Pad Ride		(Pad::ADC_1, 3     , RIDE_OUT_GPIO_Port		 , RIDE_OUT_Pin		 , Pad::Ride	  , (302  + 100/*Special case*/ ), 1527 			, Pad::CURVE_LINEAR);
Pad SideStick	(Pad::ADC_2, 0     , SIDESTICK_OUT_GPIO_Port , SIDESTICK_OUT_Pin , Pad::SideStick , (1629 + HIT_THRESHOLD_OFFSET), 4095 			, Pad::CURVE_LINEAR);
Pad Kick		(Pad::ADC_2, 1     , KICK_OUT_GPIO_Port		 , KICK_OUT_Pin		 , Pad::Kick	  , (1676 + HIT_THRESHOLD_OFFSET), 3147 			, Pad::CURVE_LINEAR);
Pad Snare		(Pad::ADC_2, 2     , SNARE_OUT_GPIO_Port	 , SNARE_OUT_Pin	 , Pad::Snare	  , (1536 + HIT_THRESHOLD_OFFSET), 2277 			, Pad::CURVE_LINEAR);
Pad MidTom		(Pad::ADC_3, 0     , MT_OUT_GPIO_Port		 , MT_OUT_Pin		 , Pad::MidTom	  , (928  + HIT_THRESHOLD_OFFSET), 3485 			, Pad::CURVE_LINEAR);
Pad LowTom		(Pad::ADC_3, 1     , LT_OUT_GPIO_Port		 , LT_OUT_Pin		 , Pad::LowTom	  , (1322 + HIT_THRESHOLD_OFFSET), 3273 			, Pad::CURVE_LINEAR);
Pad HighTom		(Pad::ADC_3, 2     , HT_OUT_GPIO_Port		 , HT_OUT_Pin		 , Pad::HighTom	  , (1381 + HIT_THRESHOLD_OFFSET), 3365 			, Pad::CURVE_LINEAR);
/**
 * @note To reduce interference in the same ADC group,
 * the sampling time of ADC channels should be set to 480 cycles.
 */

Midi midi; // MIDI communication handler

bool triggered[Pad::PAD_NUM] = { false }; // Pad trigger state array

char dbg_buf[128]; // Debug message buffer

/**
 * @brief Send debug message via UART
 * @param str Debug message string
 */
void DBG(const char* str) {
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
}

/**
 * @brief Main application entry point
 * 
 * Initializes hardware and enters main processing loop:
 * 1. Waits for power on
 * 2. Initializes ADC DMA for pad sensing
 * 3. Initializes UI components
 * 4. Enters main processing loop:
 *    - Updates UI
 *    - Processes pad hits
 *    - Handles MIDI communication
 * 
 * @return int Application exit status (never returns)
 */
int cpp_main() {
	
	ui.buttonInit(12, 400, 600, 900);
	
	while (!ui.chkPower()) { ui.buttonTick(); }

	DBG("Power on.\r\n");

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Pad::adc1_buf, ADC1_PAD_NUMS);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*)Pad::adc2_buf, ADC2_PAD_NUMS);
	HAL_ADC_Start_DMA(&hadc3, (uint32_t*)Pad::adc3_buf, ADC3_PAD_NUMS);

	DBG("OLED init...\r\n");
	ui.init();
	ui.welcome();

	// if (!oled.begin()) {
	// 	DBG("OLED Initialization Failed.\r\n");
	// 	while (true);
	// }
	// oled.printText(0, 0, "Debugging", 16);
	// oled.printText(0, 2, "OLED Test", 8);
	// for (uint8_t y = 0; y < 32; y++) {
	// 	for (uint8_t x = 0; x < 128; x++) {
	// 		oled.setPixel(x, y, true);
	// 	}
	// }
	// for (uint8_t y = 2; y < 30; y++) {
	// 	for (uint8_t x = 2; x < 126; x++) {
	// 		oled.setPixel(x, y, false);
	// 	}
	// }

	midi.isConnected() ? DBG("MIDI connected.\r\n") : DBG("MIDI not connected.\r\n");

	Pad* pads[Pad::PAD_NUM] = {
		&OpenHiHat, &CloseHiHat, &Crash, &Ride,
		&SideStick, &Kick, &Snare,
		&MidTom, &LowTom, &HighTom
	};

	DBG("Setup done, entering main loop.\r\n");
	while (ui.chkPower()) {
		ui.update();

		for (uint8_t i = 0; i < Pad::PAD_NUM; i++) {
			pads[i]->detectHit();
			if (pads[i]->isTriggered()) {
				triggered[i] = true;
			}

			if (triggered[i]) {
				pads[i]->measureForce();
			}

			if (pads[i]->isMeasurementCplt()) {
				DBG("--\r\n");

				ui.updatePadStats(pads[i]->getID(), 1);
				
				if (midi.isConnected()) {
					if (!midi.sendNoteOn(pads[i]->getID(), pads[i]->getForce(), 10)) {
						DBG("MIDI note sending failed!\r\n");
					} else {
						sprintf(dbg_buf, "MIDI Note On sent %d\r\n", pads[i]->getForce());
						DBG(dbg_buf);
						ui.updateMidiStats();
					}
				} else {
					// DBG("MIDI not connected!\r\n");
					ui.updateMidiConn(false);
				}

				// This section is for pad insts upper_limit testing. Use MaxF value for reference.
				// Hit your pads (with large force) multiple times and record the maxF value.
				// This value differs because of sensor sensitivity, drumpad fastness, and adc sampling speed.
				// Then set the pad's upper_limit to a value slightly LESS than maxF.
				//
				// sprintf(dbg_buf, "Pad %s MIDI force: %d\r\n", pads[i]->ID2Str(pads[i]->getID()), pads[i]->getForce());
				// DBG(dbg_buf);
				// static uint16_t maxF[Pad::PAD_NUM] = { 0 };
				// uint16_t peak_val = pads[i]->getPeak_DBG();
				// if (peak_val > maxF[i]) {
				// 	maxF[i] = peak_val;
				// }
				// sprintf(dbg_buf, "MaxF for %s: %d\r\n", pads[i]->ID2Str(pads[i]->getID()), maxF[i]);
				// DBG(dbg_buf);

				pads[i]->resetMeasurementCplt();
				triggered[i] = false;
			}
		}

		// Below is for ADC value waveform debugging.
		//
		// uint16_t opHihat_val, clHihat_val, crash_val, ride_val, kick_val,
		//  		 snare_val, ssdk_val, htom_val, mtom_val, ltom_val;
		// opHihat_val = OpenHiHat.getADCVal_DBG();
		// clHihat_val = CloseHiHat.getADCVal_DBG();
		// crash_val = Crash.getADCVal_DBG();
		// ride_val = Ride.getADCVal_DBG();
		// kick_val = Kick.getADCVal_DBG();
		// snare_val = Snare.getADCVal_DBG();
		// ssdk_val = SideStick.getADCVal_DBG();
		// htom_val = HighTom.getADCVal_DBG();
		// mtom_val = MidTom.getADCVal_DBG();
		// ltom_val = LowTom.getADCVal_DBG();
		// sprintf(dbg_buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
		// 		opHihat_val, clHihat_val, crash_val, ride_val, kick_val,
		// 		snare_val, ssdk_val, htom_val, mtom_val, ltom_val);
		// DBG(dbg_buf);

		if (midi.isConnected()) {
			midi.autoNoteOff();
		}
		ui.updateMidiConn(midi.isConnected());

		// HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);  // This is for debugging, to see how fast the loop runs.
	}


	while (true); // Wait for death
}
