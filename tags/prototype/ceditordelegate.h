#ifndef CEDITORDELEGATE_H
#define CEDITORDELEGATE_H

#include <QItemDelegate>

class CEditorDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  explicit CEditorDelegate(QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;

//  void updateEditorGeometry(QWidget *editor,
//                            const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
  
public slots:
  
};

#endif // CEDITORDELEGATE_H
