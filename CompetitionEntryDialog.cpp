#include "CompetitionEntryDialog.hpp"
#include "ui_CompetitionEntryDialog.h"

CompetitionEntryDialog::CompetitionEntryDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::CompetitionEntryDialog),
    competitionCreated(false)
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

QDate CompetitionEntryDialog::date(void) const
{
    return this->ui->calendarWidget->selectedDate();
}

QString CompetitionEntryDialog::place(void) const
{
    return this->ui->placeLineEdit->text();
}

float CompetitionEntryDialog::wheelRadius(void) const
{
    return this->ui->wheelDoubleSpinBox->value();
}

bool CompetitionEntryDialog::isNewlyCreated(void) const
{
    return this->competitionCreated;
}

QString CompetitionEntryDialog::competitionName(void) const
{
    return this->ui->competitionComboBox->currentText();
}

void CompetitionEntryDialog::on_buttonBox_accepted(void)
{
    if (this->ui->competitionComboBox->currentText().isEmpty())
    {
        QMessageBox::warning(this, tr("Erreur de formulaire"),
                             tr("Merci de fournir le nom de la compétition"));
        return;
    }

    // If is a new competition
    if (this->ui->competitionEntryFrame->isVisible())
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

    this->competitionCreated = false;
    this->ui->competitionEntryFrame->setVisible(this->competitionCreated);
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
    this->competitionCreated = !this->initialCompetitions.contains(text);
    this->ui->competitionEntryFrame->setVisible(this->competitionCreated);
}

void CompetitionEntryDialog::fillCompetitionBox(void)
{
    QSqlQueryModel* model = new QSqlQueryModel(this);
    model->setQuery("select name from COMPETITION");

    for (int i(0); i < model->rowCount(); i++)
        this->initialCompetitions << model->record(i).value(0).toString();

    this->ui->competitionComboBox->setModel(model);
}
