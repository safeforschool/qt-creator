/****************************************************************************
**
** Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "projects.h"

#include <QtGlobal>

#include "projectdoesnotexistsexception.h"

namespace CodeModelBackEnd {

void Projects::createOrUpdate(const QVector<ProjectContainer> &projectContainers)
{
    for (const ProjectContainer &projectContainer : projectContainers)
        createOrUpdateProject(projectContainer);
}

void Projects::remove(const Utf8StringVector &projectFilePaths)
{
    Utf8StringVector processedProjectFilePaths = projectFilePaths;

    auto removeBeginIterator = std::remove_if(projects_.begin(), projects_.end(), [&processedProjectFilePaths] (const Project &project) {
        return processedProjectFilePaths.removeFast(project.projectFilePath());
    });

    std::for_each(removeBeginIterator, projects_.end(), [](Project &project) { project.clearProjectFilePath(); });
    projects_.erase(removeBeginIterator, projects_.end());

    if (!processedProjectFilePaths.isEmpty())
        throw ProjectDoesNotExistException(processedProjectFilePaths);
}

bool Projects::hasProject(const Utf8String &projectFilePath) const
{
    return findProject(projectFilePath) != projects_.cend();
}

const Project &Projects::project(const Utf8String &projectFilePath) const
{
    const auto findIterator = findProject(projectFilePath);

    if (findIterator == projects_.cend())
        throw ProjectDoesNotExistException({projectFilePath});

    return *findIterator;
}

std::vector<Project>::const_iterator Projects::findProject(const Utf8String &projectFilePath) const
{
    return std::find_if(projects_.begin(), projects_.end(), [projectFilePath] (const Project &project) {
        return project.projectFilePath() == projectFilePath;
    });
}

std::vector<Project>::iterator Projects::findProject(const Utf8String &projectFilePath)
{
    return std::find_if(projects_.begin(), projects_.end(), [projectFilePath] (const Project &project) {
        return project.projectFilePath() == projectFilePath;
    });
}

const std::vector<Project> &Projects::projects() const
{
    return projects_;
}

void Projects::createOrUpdateProject(const ProjectContainer &projectContainer)
{
    auto findIterator = findProject(projectContainer.filePath());
    if (findIterator == projects_.cend())
        projects_.push_back(Project(projectContainer));
    else
        findIterator->setArguments(projectContainer.arguments());
}


} // namespace CodeModelbackEnd

