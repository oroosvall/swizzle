# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####


from operator import itemgetter

def get_bind_pose(bones, vgroups):

    bone_list = []

    for bo in bones:
        vgrp = vgroups.get(bo.name)

        if vgrp:
            print(bo.name)
            idx = vgrp.index
            bone_list.append([idx, -1, bo])

    bone_list.sort(key=itemgetter(0))


    for idx, _, bo in bone_list:
        if bo.parent:
            for pidx, _, pbo in bone_list:
                if bo.parent.name == pbo.name:
                    bone_list[idx][1] = pidx
                    break
        else:
            bone_list[idx][1] = idx

    return bone_list


def get_matrix(bone):
    bo = bone[2]
    mat = bo.matrix_local
    if bo.parent:
        pMat = bo.parent.matrix_local
        mat = pMat.inverted() @ mat
    return mat


def get_matrix_2(bone, global_matrix):
    bo = bone[2]
    mat = bo.matrix
    if bo.parent:
        pMat = bo.parent.matrix
        mat = pMat.inverted() @ mat
    else:
        mat = global_matrix @ mat
    return mat


def get_matrix_3(bone):
    bo = bone[2]
    mat = bo.matrix
    if bo.parent:
        pMat = bo.parent.matrix
        mat = pMat.inverted() @ mat
    return mat


def get_pose(bones, vgroups, matrix):

    bone_list = []

    for bo in bones:
        mat = bo.matrix
        if bo.parent:
            pMat = bo.parent.matrix
            mat = pMat.inverted() @ mat
            vgrp = vgroups.get(bo.name)
            if vgrp:
                idx = vgrp.index
                bone_list.append([idx, mat])
        else:
            mat = matrix @ mat
            vgrp = vgroups.get(bo.name)
            if vgrp:
                idx = vgrp.index
                bone_list.append([idx, mat])

    bone_list.sort(key=itemgetter(0))

    return bone_list
