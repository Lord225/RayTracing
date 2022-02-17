import numpy as np

import pyrr


class Camera:
    def __init__(self, pos, dir, aspectratio, focal):
        proj = pyrr.matrix44.create_perspective_projection(90, aspectratio, 0.2, 1)
        view = pyrr.matrix44.create_look_at(np.array([0,0,0]), dir/np.linalg.norm(dir), np.array([0.0, 1.0, 0.0]))

        self.inv = np.linalg.inv(np.matmul(proj, view))
        self.pos = pos

    def gen_rays(self, uvs):
        out = np.concatenate([uvs, np.ones((uvs.shape[0], 2))], axis=1) # [[u,v]...] (Nx2) => [[u, v, 1, 1] ...] (Nx4) => (4xN)
        dirs = np.matmul(out, self.inv.T) # [[u, v, 1, 1] ...] * inv => [[x, y, z, _] ...] (4xN)
        dirs = np.delete(dirs, -1, axis=1) #  [[x, y, z] ...] (3xN)

        norms = np.reshape(np.linalg.norm(dirs, axis=1), (-1, 1))
        dirs = dirs/norms

        poss = np.reshape(np.tile(self.pos, dirs.shape[0]), (-1, 3)) # [[px, py, pz], [px, py, pz] ...] (3xN)
        rays = np.reshape(np.concatenate([dirs, poss], axis=1), (-1, 2, 3)) # [[[x, y, z], [px, py, pz]], [[x, y, z], [px, py, pz]] ...] (N, 2, 3) (ray, dir/pos, xyz)
        return rays