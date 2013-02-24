#ifndef __COMPETITIONENTRYDIALOG_HPP__
#define __COMPETITIONENTRYDIALOG_HPP__

#include <QtGui>
#include <QtSql>

namespace Ui {
class CompetitionEntryDialog;
}

class CompetitionEntryDialog : public QDialog
{
    Q_OBJECT
    
    public:

        explicit CompetitionEntryDialog(QWidget* parent = 0);
        ~CompetitionEntryDialog();

        // Getter
        QString competitionName(void) const;
        float wheelRadius(void) const;
        QString place(void) const;
        QDate date(void) const;
        bool isNewlyCreated(void) const;

    private slots:

        // Autoconnect
        void on_buttonBox_accepted(void);
        void on_buttonBox_rejected(void);
        void on_competitionComboBox_activated(int index);
        void on_competitionComboBox_editTextChanged(const QString& text);

private:

        void fillCompetitionBox(void);

    protected:

        Ui::CompetitionEntryDialog* ui;

        bool competitionCreated;
        QStringList initialCompetitions;
};

#endif /* __COMPETITIONENTRYDIALOG_HPP__ */
