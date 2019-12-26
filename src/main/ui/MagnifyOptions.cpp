/************************************************************************************/
/* An OpenCV/Qt based realtime application to magnify motion and color              */
/* Copyright (C) 2015  Jens Schindel <kontakt@jens-schindel.de>                     */
/*                                                                                  */
/* Based on the work of                                                             */
/*      Joseph Pan      <https://github.com/wzpan/QtEVM>                            */
/*      Nick D'Ademo    <https://github.com/nickdademo/qt-opencv-multithreaded>     */
/*                                                                                  */
/* Realtime-Video-Magnification->MagnifyOptions.cpp                                 */
/*                                                                                  */
/* This program is free software: you can redistribute it and/or modify             */
/* it under the terms of the GNU General Public License as published by             */
/* the Free Software Foundation, either version 3 of the License, or                */
/* (at your option) any later version.                                              */
/*                                                                                  */
/* This program is distributed in the hope that it will be useful,                  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of                   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    */
/* GNU General Public License for more details.                                     */
/*                                                                                  */
/* You should have received a copy of the GNU General Public License                */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.            */
/************************************************************************************/

#include "main/ui/MagnifyOptions.h"
#include "ui_MagnifyOptions.h"

MagnifyOptions::MagnifyOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MagnifyOptions)
{
    // Setup Options Widget
    ui->setupUi(this);

    //Create new double slider
    doubleSlider = new QxtSpanSlider(Qt::Horizontal,this);
    ui->doubleSliderField->insertWidget(0, doubleSlider);
    //doubleSlider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);


    // Connect all sliders/buttons/boxes directly for responsible feeling
    connect(ui->MagnifcationtypeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(updateFlagsFromOptionsTab()));
    connect(ui->MagnifcationtypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(reset(int)));

    // Update Settings
    connect(ui->COLowDoubleSpinBox, SIGNAL(valueChanged(double)), SLOT(updateSettingsFromOptionsTab()));
    connect(ui->COHighDoubleSpinBox, SIGNAL(valueChanged(double)), SLOT(updateSettingsFromOptionsTab()));
    connect(ui->ChromSpinBox, SIGNAL(valueChanged(int)), SLOT(updateSettingsFromOptionsTab()));
    connect(ui->AmplificationSpinBox, SIGNAL(valueChanged(int)), SLOT(updateSettingsFromOptionsTab()));
    connect(ui->COWavelengthSpinBox, SIGNAL(valueChanged(double)), SLOT(updateSettingsFromOptionsTab()));
    connect(ui->LevelsSpinBox, SIGNAL(valueChanged(int)), SLOT(updateSettingsFromOptionsTab()));
    connect(ui->ThresholdSpinBox, SIGNAL(valueChanged(double)), SLOT(updateSettingsFromOptionsTab()));

    // Update Spinbox
    connect(ui->COWavelengthSlider, SIGNAL(valueChanged(int)), this, SLOT(convertFromSlider(int)));
    connect(doubleSlider, SIGNAL(lowerPositionChanged(int)), this, SLOT(convertFromSlider(int)));
    connect(doubleSlider, SIGNAL(upperPositionChanged(int)), this, SLOT(convertFromSlider(int)));
    connect(ui->ChromSlider, SIGNAL(valueChanged(int)), this, SLOT(convertFromSlider(int)));
    connect(ui->ThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(convertFromSlider(int)));

    // Update Slider
    connect(ui->COWavelengthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(convertFromSpinBox(double)));
    connect(ui->COLowDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(convertFromSpinBox(double)));
    connect(ui->COHighDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(convertFromSpinBox(double)));
    connect(ui->ThresholdSpinBox, SIGNAL(valueChanged(double)), this, SLOT(convertFromSpinBox(double)));

    // Other
    connect(ui->resetButton, SIGNAL(clicked()), SLOT(reset()));
    connect(ui->grayscaleCheckBox, SIGNAL(clicked()), SLOT(updateFlagsFromOptionsTab()));

    // Initialize Settings with Default values
    ui->MagnifcationtypeComboBox->setCurrentIndex(DEFAULT_MAGNIFY_TYPE);
    reset();
}

MagnifyOptions::~MagnifyOptions()
{
    delete ui;
}

// Internal slots supporting GUI
void MagnifyOptions::convertFromSpinBox(double val)
{
    if(imgProcFlags.colorMagnifyOn) {
        if(sender() == ui->COLowDoubleSpinBox)
            doubleSlider->setLowerValue((int)(val*100.f));
        if(sender() == ui->COHighDoubleSpinBox)
            doubleSlider->setUpperValue((int)(val*100.f));
    }
    else {
        if(sender() == ui->COLowDoubleSpinBox)
            doubleSlider->setLowerValue((int)val);
        if(sender() == ui->COHighDoubleSpinBox)
            doubleSlider->setUpperValue((int)val);
    }
    if(sender() == ui->COWavelengthSpinBox)
        ui->COWavelengthSlider->setValue((int)(val*10.0));
    if(sender() == ui->ThresholdSpinBox)
        ui->ThresholdSlider->setValue((int)(val*10.0));
}

void MagnifyOptions::convertFromSlider(int val)
{
    if(sender() == doubleSlider) {
        if(imgProcFlags.colorMagnifyOn) {
            if(val == doubleSlider->lowerPosition())
                ui->COLowDoubleSpinBox->setValue((double) val/100.f);
            else if( val == doubleSlider->upperPosition())
                ui->COHighDoubleSpinBox->setValue((double) val/100.f);
        }
        else {
            if(val == doubleSlider->lowerPosition())
                ui->COLowDoubleSpinBox->setValue((double) val);
            else if( val == doubleSlider->upperPosition())
                ui->COHighDoubleSpinBox->setValue((double) val);
        }
    }
    if(sender() == ui->COWavelengthSlider)
        ui->COWavelengthSpinBox->setValue((double)val/10.0);
    if(sender() == ui->ThresholdSlider)
        ui->ThresholdSpinBox->setValue((double)val/10.0);
}

// Reset everything to Default, configured in Config.h, depending on Magnify Type. On Start its 0
void MagnifyOptions::reset()
{
    int magnifyType = ui->MagnifcationtypeComboBox->currentIndex();
    switch (magnifyType) {
    case 1:
        applyColorInterface();
        ui->LevelsSpinBox->setValue(DEFAULT_COL_MAG_LEVELS);
        ui->AmplificationSpinBox->setValue(DEFAULT_CM_AMPLIFICATION);
        ui->AmplificationSlider->setValue(DEFAULT_CM_AMPLIFICATION);
        ui->COWavelengthSpinBox->setValue(DEFAULT_CM_COWAVELENGTH);
        ui->COWavelengthSlider->setValue(DEFAULT_CM_COWAVELENGTH);
        ui->COLowDoubleSpinBox->setValue(DEFAULT_CM_COLOW);
        doubleSlider->setLowerValue((int)(DEFAULT_CM_COLOW*100.f));
        ui->COHighDoubleSpinBox->setValue(DEFAULT_CM_COHIGH);
        doubleSlider->setUpperValue((int)(DEFAULT_CM_COHIGH*100.f));
        ui->ChromSpinBox->setValue(DEFAULT_CM_CHROMATTENUATION);
        ui->ChromSlider->setValue(DEFAULT_CM_CHROMATTENUATION);
        break;
    case 2:
        applyMotionInterface();
        ui->LevelsSpinBox->setValue(DEFAULT_LAP_MAG_LEVELS);
        ui->AmplificationSpinBox->setValue(DEFAULT_MM_AMPLIFICATION);
        ui->AmplificationSlider->setValue(DEFAULT_MM_AMPLIFICATION);
        ui->COWavelengthSpinBox->setValue(DEFAULT_MM_COWAVELENGTH);
        ui->COWavelengthSlider->setValue(DEFAULT_MM_COWAVELENGTH);
        ui->COLowDoubleSpinBox->setValue(DEFAULT_MM_COLOW);
        doubleSlider->setLowerValue((int)(DEFAULT_MM_COLOW));
        ui->COHighDoubleSpinBox->setValue(DEFAULT_MM_COHIGH);
        doubleSlider->setUpperValue((int)(DEFAULT_MM_COHIGH));
        ui->ChromSpinBox->setValue(DEFAULT_MM_CHROMATTENUATION);
        ui->ChromSlider->setValue(DEFAULT_MM_CHROMATTENUATION);
        ui->ThresholdSpinBox->setValue(DEFAULT_MM_THRESHOLD);
        ui->ThresholdSlider->setValue(DEFAULT_MM_THRESHOLD);
        break;
    case 3:
        applyMotionInterface();
        ui->LevelsSpinBox->setValue(DEFAULT_DWT_MAG_LEVELS);
        ui->AmplificationSpinBox->setValue(DEFAULT_MM_AMPLIFICATION);
        ui->AmplificationSlider->setValue(DEFAULT_MM_AMPLIFICATION);
        ui->COWavelengthSpinBox->setValue(DEFAULT_MM_COWAVELENGTH);
        ui->COWavelengthSlider->setValue(DEFAULT_MM_COWAVELENGTH);
        ui->COLowDoubleSpinBox->setValue(DEFAULT_MM_COLOW);
        doubleSlider->setLowerValue((int)(DEFAULT_MM_COLOW));
        ui->COHighDoubleSpinBox->setValue(DEFAULT_MM_COHIGH);
        doubleSlider->setUpperValue((int)(DEFAULT_MM_COHIGH));
        ui->ChromSpinBox->setValue(DEFAULT_MM_CHROMATTENUATION);
        ui->ChromSlider->setValue(DEFAULT_MM_CHROMATTENUATION);
        ui->ThresholdSpinBox->setValue(DEFAULT_MM_THRESHOLD);
        ui->ThresholdSlider->setValue(DEFAULT_MM_THRESHOLD);
        break;
    default:  
        ui->LevelsSpinBox->setDisabled(true);
        ui->verticalSpacer->changeSize(0,0,QSizePolicy::Maximum, QSizePolicy::Maximum);

        ui->AmplificationLabel->hide();
        ui->AmplificationSlider->hide();
        ui->AmplificationSpinBox->hide();
        ui->AmplificationValLabel->hide();

        ui->COWavelengthLabel->hide();
        ui->COWavelengthSlider->hide();
        ui->COWavelengthSpinBox->hide();
        ui->COWavelengthValLabel->hide();

        ui->DoubleSliderLabel->hide();
        ui->COHighDoubleSpinBox->hide();
        ui->COLowDoubleSpinBox->hide();
        doubleSlider->hide();
        ui->DoubleSliderValLabel->hide();

        ui->ChromSpinBox->hide();
        ui->ChromSlider->hide();
        ui->ChromLabel->hide();
        ui->ChromValLabel->hide();

        ui->BpmLabel->hide();
        ui->BpmSpacer->hide();
        ui->loBpm->hide();
        ui->hiBpm->hide();
        ui->HzSpacer->hide();

        ui->resetButton->hide();

        break;
    }
}
// This is for handling the combobox
void MagnifyOptions::reset(int type)
{
    reset();
}

void MagnifyOptions::updateFlagsFromOptionsTab()
{
    // Grayscale
    imgProcFlags.grayscaleOn = ui->grayscaleCheckBox->isChecked();
    // What Magnification Type was chosen?
    imgProcFlags.colorMagnifyOn = (ui->MagnifcationtypeComboBox->currentIndex() == 1);
    imgProcFlags.laplaceMagnifyOn = (ui->MagnifcationtypeComboBox->currentIndex() == 2);
    imgProcFlags.waveletMagnifyOn = (ui->MagnifcationtypeComboBox->currentIndex() == 3);

    emit newImageProcessingFlags(imgProcFlags);
}

void MagnifyOptions::updateSettingsFromOptionsTab()
{
    imgProcSettings.amplification = ui->AmplificationSpinBox->value();
    imgProcSettings.coWavelength = ui->COWavelengthSpinBox->value()*10.0;
    if(imgProcFlags.colorMagnifyOn) {
        imgProcSettings.coLow = ui->COLowDoubleSpinBox->value();
        imgProcSettings.coHigh = ui->COHighDoubleSpinBox->value();

        int hiBPM = imgProcSettings.coHigh*60.0;
        int loBPM = imgProcSettings.coLow*60.0;

        QString loBpm = QString::number(loBPM);
        QString hiBpm = QString::number(hiBPM);

        ui->loBpm->setText(loBpm);
        ui->hiBpm->setText(hiBpm);
    }
    else {
        imgProcSettings.coLow = ui->COLowDoubleSpinBox->value()/100.0;
        imgProcSettings.coHigh = ui->COHighDoubleSpinBox->value()/100.0;
    }
    imgProcSettings.chromAttenuation = (double)ui->ChromSpinBox->value()/100.0;
    imgProcSettings.levels = ui->LevelsSpinBox->value();
    imgProcSettings.threshold = ui->ThresholdSpinBox->value();

    emit newImageProcessingSettings(imgProcSettings);
}

ImageProcessingSettings MagnifyOptions::getSettings()
{
    return this->imgProcSettings;
}

ImageProcessingFlags MagnifyOptions::getFlags()
{
    return this->imgProcFlags;
}

void MagnifyOptions::applyColorInterface()
{
    ui->LevelsSpinBox->setDisabled(false);
    ui->verticalSpacer->changeSize(0,20,QSizePolicy::Maximum, QSizePolicy::Maximum);

    doubleSlider->setMaximum(300);
    ui->COHighDoubleSpinBox->setMaximum(3.0);
    ui->COLowDoubleSpinBox->setMaximum(3.0);

    ui->AmplificationLabel->show();
    ui->AmplificationSlider->show();
    ui->AmplificationSpinBox->show();
    ui->AmplificationValLabel->show();

    ui->COWavelengthLabel->hide();
    ui->COWavelengthSlider->hide();
    ui->COWavelengthSpinBox->hide();
    ui->COWavelengthValLabel->hide();

    ui->DoubleSliderLabel->show();
    ui->COHighDoubleSpinBox->show();
    ui->COLowDoubleSpinBox->show();
    doubleSlider->show();
    ui->DoubleSliderValLabel->show();

    ui->ChromSpinBox->hide();
    ui->ChromSlider->hide();
    ui->ChromLabel->hide();
    ui->ChromValLabel->hide();

    ui->BpmLabel->show();
    ui->BpmSpacer->show();
    ui->loBpm->show();
    ui->hiBpm->show();
    ui->HzSpacer->show();
    ui->DoubleSliderValLabel->setText("Hz");

    ui->resetButton->show();
}

void MagnifyOptions::applyMotionInterface()
{
    ui->LevelsSpinBox->setDisabled(false);
    ui->verticalSpacer->changeSize(0,20,QSizePolicy::Maximum, QSizePolicy::Maximum);

    doubleSlider->setMaximum(100);
    ui->COHighDoubleSpinBox->setMaximum(100.0);
    ui->COLowDoubleSpinBox->setMaximum(100.0);

    ui->AmplificationLabel->show();
    ui->AmplificationSlider->show();
    ui->AmplificationSpinBox->show();
    ui->AmplificationValLabel->show();

    ui->COWavelengthLabel->show();
    ui->COWavelengthSlider->show();
    ui->COWavelengthSpinBox->show();
    ui->COWavelengthValLabel->show();

    ui->DoubleSliderLabel->show();
    ui->COHighDoubleSpinBox->show();
    ui->COLowDoubleSpinBox->show();
    doubleSlider->show();
    ui->DoubleSliderValLabel->show();

    ui->ChromSpinBox->show();
    ui->ChromSlider->show();
    ui->ChromLabel->show();
    ui->ChromValLabel->show();

    ui->BpmLabel->hide();
    ui->BpmSpacer->hide();
    ui->loBpm->hide();
    ui->hiBpm->hide();
    ui->HzSpacer->show();
    ui->DoubleSliderValLabel->setText("%");

    ui->resetButton->show();
}

void MagnifyOptions::toggleGrayscale(bool isActive)
{
    ui->grayscaleCheckBox->setDisabled(!isActive);
}

void MagnifyOptions::setMaxLevel(int level)
{
    if(ui->LevelsSpinBox->value() > level) {
        ui->LevelsSpinBox->setValue(level);
    }
    ui->LevelsSpinBox->setMaximum(level);
}
