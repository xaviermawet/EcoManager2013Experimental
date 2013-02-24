#include "CompetitionEntryDialog.hpp"
#include "ui_CompetitionEntryDialog.h"

CompetitionEntryDialog::CompetitionEntryDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::CompetitionEntryDialog)
{
    // GUI Configuration
    this->ui->setupUi(this);

    // unable to create a competition at a future date
    this->ui->calendarWidget->setMaximumDate(QDate::currentDate());

    // Add items to the competition comboBox
    this->fillCompetitionBox();
}

CompetitionEntryDialog::~CompetitionEntryDialog()
{
    delete this->ui;
}

QString CompetitionEntryDialog::competitionName(void) const
{
    return this->ui->competitionComboBox->currentText();
}

float CompetitionEntryDialog::wheelRadius(void) const
{
    return this->ui->wheelDoubleSpinBox->value();
}

QString CompetitionEntryDialog::place(void) const
{
    return this->ui->placeLineEdit->text();
}

QDate CompetitionEntryDialog::date(void) const
{
    return this->ui->calendarWidget->selectedDate();
}

bool CompetitionEntryDialog::isNewlyCreated(void) const
{
    return this->competitionCreated;
}

void CompetitionEntryDialog::on_buttonBox_accepted(void)
{
    if (this->ui->placeLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Erreur de formulaire"),
                             tr("Merci de fournir le nom de la compétition"));
        return;
    }

    // If is a new competition
    if (this->ui->competitionEntry->isVisible())
    {
        if (this->ui->placeLineEdit->text().isEmpty())
        {
            QMessageBox::warning(this, tr("Erreur de formulaire"),
                      tr("Merci de fournir la localisation de la compétition"));
            return;
        }

        if (this->ui->wheelDoubleSpinBox->value() == 0.0)
        {
            QMessageBox::warning(this, tr("Erreur de formulaire"),
                                 tr("Merci de fournir le tour de la roue"));
            return;
        }
    }

    this->accept();
}

void CompetitionEntryDialog::on_buttonBox_rejected(void)
{
    this->reject();
}

// existingCompetitionSelected
void CompetitionEntryDialog::on_competitionComboBox_activated(int index)
{
    Q_UNUSED(index);

    this->_competitionCreated = false;
    this->ui->competitionEntry->setVisible(this->_competitionCreated);
}

// competitionEditing
void CompetitionEntryDialog::on_competitionComboBox_editTextChanged(const QString& text)
{
    /*
    if (initialCompetitions.contains(text))
    {
        this->_competitionCreated = false;
        this->ui->competitionEntry->setVisible(false);
    }
    else
    {
        this->_competitionCreated = true;
        this->ui->competitionEntry->setVisible(true);
    }
    */

    // Display form if the competition doesn't exist
    this->_competitionCreated = !this->_initialCompetitions.contains(text);
    this->ui->competitionEntry->setVisible(this->_competitionCreated);
}

void CompetitionEntryDialog::fillCompetitionBox(void)
{
    QSqlQueryModel* model = new QSqlQueryModel(this);
    model->setQuery("select name from COMPETITION");

    for (int i(0); i < model->rowCount(); i++)
        this->initialCompetitions << model->record(i).value(0).toString();

    this->ui->competitionComboBox->setModel(model);
}
