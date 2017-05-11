#pragma once

#include <obs.hpp>
#include <QPointer>
#include <QDialog>
#include <QLabel>
#include <QList>

class QGridLayout;

class OBSBasicStats : public QDialog {
	Q_OBJECT

	QLabel *cpuUsage = nullptr;
	QLabel *hddSpace = nullptr;
	QLabel *memUsage = nullptr;

	QLabel *renderTime = nullptr;
	QLabel *skippedFrames = nullptr;
	QLabel *missedFrames = nullptr;

	QGridLayout *outputLayout = nullptr;

	struct OutputLabels {
		QPointer<QLabel> name;
		QPointer<QLabel> status;
		QPointer<QLabel> disconnections;
		QPointer<QLabel> droppedFrames;
		QPointer<QLabel> metabytesSent;
		QPointer<QLabel> bitrate;
	};

	QList<OutputLabels> outputLabels;

	void AddOutputLabels(QString name);

public:
	OBSBasicStats(QWidget *parent = nullptr);
};
